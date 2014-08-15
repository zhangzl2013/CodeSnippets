/**
 * @file	client.c
 * @brief	Http CONNECT wraper
 * @author	Zhang Zhaolong <zhangzl2013@126.com>
 * @data	2013-07-23
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include "common.h"
#ifdef _WIN32_
#include <winsock.h>
#else
#include <netdb.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#endif
#include "tunnel.h"

typedef struct {
	char *me;
	char *host_name;
	int host_port;
	char *proxy_name;
	int proxy_port;
	int forward_port;
	char *user_agent;
	int is_https;
} Arguments;

static void usage(FILE * f, const char *me)
{
	fprintf(f,
			"Usage: %s [OPTION]... HOST:PORT\n"
			"Connect to a http tunnel proxy with CONNECT method, set up a http tunnel\n"
			"connection to PORT at HOST.\n"
			"When a connection is made, I/O is redirected from the source specified\n"
			"by the --forward-port FORWARD_PORT to the tunnel.\n"
			"\n"
			"  -h, --help                               display this help and exit\n"
			"  -V, --version                            output version information and exit\n"
			"  -F, --forward-port PORT                  [Required] use TCP port PORT for input and output\n"
			"  -P, --proxy http[s]://HOSTNAME:PORT      [Required] use a HTTP proxy\n"
			"  -U, --user-agent STRING                  specify User-Agent value in HTTP requests\n"
			"\n" "Report bugs to %s.\n", me, BUG_REPORT_EMAIL);
}

static void name_and_port(const char *nameport, char **name, int *port)
{
	char *p;

	*name = strdup(nameport);
	if (*name == NULL) {
		fprintf(stderr, "Out of memory\n");
		exit(1);
	}

	p = strchr(*name, ':');
	if (p != NULL) {
		*port = atoi(p + 1);
		*p = '\0';
	}
	DBG("name: %s, port: %d", *name, *port);
}

static void parse_arguments(int argc, char **argv, Arguments * arg)
{
	int i;

	arg->me = argv[0];
	arg->forward_port = -1;
	arg->host_name = NULL;
	arg->host_port = -1;
	arg->proxy_name = NULL;
	arg->proxy_port = -1;
	arg->user_agent = NULL;
	arg->is_https = 0;

	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
			usage(stdout, arg->me);
			exit(0);
		}
		else if (strcmp(argv[i], "--version") == 0 || strcmp(argv[i], "-v") == 0) {
			printf("Http Tunnel Client (%s)\n", VERSION);
			exit(0);
		}
		else if (strcmp(argv[i], "--proxy") == 0 || strcmp(argv[i], "-P") == 0) {
			i++;
			if (strncmp(argv[i], "https", 5) == 0) {
#ifdef _OPENSSL_
				arg->is_https = 1;
				name_and_port(argv[i] + 8, &arg->proxy_name, &arg->proxy_port);
#else
				fprintf(stderr, "%s: SSL connection is not supported by this configuration.\n"
						"%s: try '%s --help' for help.\n", arg->me, arg->me, arg->me);
				exit(1);
#endif
			}
			else if (strncmp(argv[i], "http", 4) == 0) {
				arg->is_https = 0;
				name_and_port(argv[i] + 7, &arg->proxy_name, &arg->proxy_port);
			}
			else {
				fprintf(stderr, "%s: argument for --proxy is incorrect.\n"
						"%s: try '%s --help' for help.\n", arg->me, arg->me, arg->me);
				exit(1);
			}
		}
		else if (strcmp(argv[i], "--forward-port") == 0 || strcmp(argv[i], "-F") == 0) {
			i++;
			arg->forward_port = atoi(argv[i]);
		}
		else if (strncmp(argv[i], "-", 1) != 0) {
			name_and_port(argv[i], &arg->host_name, &arg->host_port);
		}
		else {
			DBG("nani?");
		}
	}

	if (arg->forward_port == -1) {
		fprintf(stderr, "%s: the --forward_port must be specified.\n"
				"%s: try '%s --help' for help.\n", arg->me, arg->me, arg->me);
		exit(1);
	}

	if (arg->proxy_name == NULL || arg->proxy_port == -1) {
		fprintf(stderr, "%s: the --proxy must be specified.\n"
				"%s: try '%s --help' for help.\n", arg->me, arg->me, arg->me);
		exit(1);
	}

	DBG("%s started with arguments:", arg->me);
	DBG(" me = %s", arg->me);
	DBG(" host_name = %s", arg->host_name ? arg->host_name : "(null)");
	DBG(" host_port = %d", arg->host_port);
	DBG(" proxy_name = %s", arg->proxy_name ? arg->proxy_name : "(null)");
	DBG(" proxy_port = %d", arg->proxy_port);
	DBG(" forward_port = %d", arg->forward_port);
	DBG(" user_agent = %s", arg->user_agent ? arg->user_agent : "(null)");
	DBG(" is_https = %d", arg->is_https);
}

static int create_socket(int port, int backlog)
{
	struct sockaddr_in address;
	int i, s;

	s = socket(PF_INET, SOCK_STREAM, 0);
	if (s == -1) {
		ERR("create socket failed.");
		return -1;
	}

	i = 1;
	if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (void *)&i, sizeof i) == -1) {
		WARN("setsockopr SO_REUSEADDR: %s", strerror(errno));
	}

	address.sin_family = PF_INET;
	address.sin_port = htons((short)port);
	address.sin_addr.s_addr = INADDR_ANY;

	if (bind(s, (struct sockaddr *)&address, sizeof(address)) == -1) {
		_socket_close(s);
		ERR("bind socket failed.");
		return -1;
	}

	if (listen(s, (unsigned)backlog) == -1) {
		_socket_close(s);
		ERR("listen socket failed.");
		return -1;
	}
	DBG("socket: %d", s);
	return s;
}

static int wait_for_connection_on_socket(int s)
{
	struct sockaddr addr;
	int len;
	int t;

	len = sizeof addr;
	t = accept(s, &addr, &len);
	if (t == -1)
		return -1;

	return t;
}

static INLINE void handle_input(const char *type, Tunnel * tunnel, int fd, int fd_isset,
							int (*handler) (Tunnel * tunnel, int fd), int *closed)
{
	if (fd_isset) {
		int n;

		n = handler(tunnel, fd);
		if (n == 0 || (n == -1 && errno != EAGAIN)) {
			if (n == 0)
				DBG("%s closed", type);
			else
				DBG("%s read error: %s", type, strerror(errno));
			*closed = 1;
		}
	}
}

int handle_local_input(Tunnel * tunnel, int fd)
{
	unsigned char buf[10240];
	int n, m;


	n = recv(fd, buf, sizeof buf, 0);
	if (n == 0 || n == -1) {
		if (n == -1 && errno != EAGAIN)
			DBG("recv() error: %s", strerror(errno));
		return n;
	}

	m = tunnel_write(tunnel, buf, n);
	DBG("tunnel_write (%p, %p, %d) = %d", tunnel, buf, (int)n, (int)m);
	return m;
}

int handle_tunnel_input(Tunnel * tunnel, int fd)
{
	unsigned char buf[10240];
	int n, m;

	n = tunnel_read(tunnel, buf, sizeof buf);
	if (n <= 0) {
		DBG("handle_tunnel_input: tunnel_read() = %d\n", (int)n);
		if (n == -1 && errno != EAGAIN)
			DBG("handle_tunnel_input: tunnel_read() error: %s",
				strerror(errno));
		return n;
	}

	m = write_all(fd, buf, n);
	DBG("write_all (%d, %p, %d) = %d", fd, buf, (int)n, (int)m);
	return m;
}

static INLINE void ignore_sigpipe()
{
#ifndef _WIN32_
	signal(SIGPIPE, SIG_IGN);
#endif
}

static INLINE int wsa_startup()
{
#ifdef _WIN32_
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(1, 1);
	err = WSAStartup(wVersionRequested, &wsaData);

	return err ? -1 : 0;
#else
	return 0;
#endif
}

static INLINE int wsa_cleanup()
{
#ifdef _WIN32_
	int err;

	err = WSACleanup();

	return err ? -1 : 0;
#else
	return 0;
#endif
}

int main(int argc, char **argv)
{
	int s = -1;
	int fd = -1;
	int closed;
	Arguments arg;
	Tunnel *tunnel;

	parse_arguments(argc, argv, &arg);

	if (wsa_startup() == -1) {
		DBG("init_wsa failed.");
		exit(1);
	}

	s = create_socket(arg.forward_port, 0);
	if (s == -1) {
		DBG("couldn't create local socket: %s", strerror(errno));
		exit(1);
	}

	ignore_sigpipe();

	for (;;) {
		fd = wait_for_connection_on_socket(s);
		if (fd == -1) {
			DBG("couldn't forward port %d: %s",
				arg.forward_port, strerror(errno));
			exit(1);
		}

		tunnel = tunnel_new(arg.host_name, arg.host_port,
							arg.proxy_name, arg.proxy_port, arg.is_https);
		if (tunnel == NULL) {
			DBG("couldn't create tunnel");
			exit(1);
		}

		if (tunnel_connect(tunnel) == -1) {
			DBG("couldn't open tunnel: %s", strerror(errno));
			exit(1);
		}

		closed = 0;
		while (!closed) {
			fd_set inputs;
			int n;

			FD_ZERO(&inputs);
			FD_SET(fd, &inputs);
			FD_SET(tunnel->fd, &inputs);

			n = select(FD_SETSIZE, &inputs, NULL, NULL, NULL);
			if (n == -1) {
				DBG("poll error: %s", strerror(errno));
				exit(1);
			} else if (n == 0) {
				continue;
			}
			handle_input("port", tunnel, fd, FD_ISSET(fd,&inputs),
						handle_local_input, &closed);
			handle_input("port", tunnel, fd, FD_ISSET(tunnel->fd,&inputs),
						handle_tunnel_input, &closed);
		}

		_socket_close(fd);
		tunnel_destroy(tunnel);
	}

	_socket_close(s);

	wsa_cleanup();

	exit(0);
}
