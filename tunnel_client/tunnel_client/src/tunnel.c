/**
 * @file	tunnel.c
 * @brief	Http tunnel
 * @author	Zhang Zhaolong <zhangzl2013@126.com>
 * @data	2013-07-24
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include "common.h"
#ifdef _WIN32_
#include <winsock.h>
#else
#include <netdb.h>
#include <sys/socket.h>
#endif
#include "tunnel.h"

static int set_address(struct sockaddr_in *address, const char *host, int port)
{
	address->sin_family = PF_INET;
	address->sin_port = htons((short)port);
	address->sin_addr.s_addr = inet_addr(host);

	if (address->sin_addr.s_addr == INADDR_NONE) {
		struct hostent *ent;
		unsigned int ip;

		DBG("set_address: gethostbyname (\"%s\")", host);
		ent = gethostbyname(host);
		DBG("set_address: ent = %p", ent);
		if (ent == 0)
			return -1;

		memcpy(&address->sin_addr.s_addr, ent->h_addr, (unsigned)ent->h_length);
		ip = ntohl(address->sin_addr.s_addr);
		DBG("set_address: host = %d.%d.%d.%d",
			ip >> 24, (ip >> 16) & 0xff, (ip >> 8) & 0xff, ip & 0xff);
	}

	return 0;
}

#ifdef _OPENSSL_
static void ssl_init()
{
	static int initialized = 0;

	if (!initialized) {
		SSL_library_init();
		SSL_load_error_strings();
		initialized = 1;
	}
}
#endif

Tunnel *tunnel_new(const char *host, int host_port,
				   const char *proxy, int proxy_port, int is_https)
{
	const char *remote;
	int remote_port;
	Tunnel *tunnel;

	tunnel = malloc(sizeof(Tunnel));
	if (tunnel == NULL) {
		return NULL;
	}

	tunnel->fd = -1;
	tunnel->dest.host_name = host;
	tunnel->dest.host_port = host_port;
	tunnel->dest.proxy_name = proxy;
	tunnel->dest.proxy_port = proxy_port;
	tunnel->dest.user_agent = NULL;
	tunnel->is_https = is_https;
#ifdef _OPENSSL_
	tunnel->ssl_ctx = NULL;
	tunnel->ssl = NULL;
#endif
	remote = tunnel->dest.proxy_name;
	remote_port = tunnel->dest.proxy_port;

	if (set_address(&tunnel->address, remote, remote_port) == -1) {
		free(tunnel);
		return NULL;
	}
#ifdef _OPENSSL_
	if (tunnel->is_https) {
		ssl_init();

		tunnel->ssl_ctx = SSL_CTX_new(SSLv3_method());
		if (!tunnel->ssl_ctx)
		{
			DBG("SSL_CTX_new failed.");
			free(tunnel);
			return NULL;
		}

		SSL_CTX_set_options(tunnel->ssl_ctx, SSL_OP_ALL);

		tunnel->ssl = SSL_new(tunnel->ssl_ctx);
		if (!tunnel->ssl)
		{
			DBG("SSL_new failed.");
			SSL_CTX_free(tunnel->ssl_ctx);
			free(tunnel);
			return NULL;
		}
	}
#endif
	return tunnel;
}

void tunnel_delete(Tunnel * tunnel)
{
#ifdef _OPENSSL_
	if (tunnel->ssl) {
		SSL_free(tunnel->ssl);
	}

	if (tunnel->ssl_ctx) {
		SSL_CTX_free(tunnel->ssl_ctx);
	}
#endif
	free(tunnel);
}

void tunnel_destroy(Tunnel * tunnel)
{
	if (tunnel->fd != -1)
		tunnel_disconnect(tunnel);

	tunnel_delete(tunnel);
}

static int tunnel_send_CONNECT_request(Tunnel * tunnel)
{
	char buf[1024];
	int n;

	sprintf(buf, "%s %s:%d %s %s:%d%s", "CONNECT",
			tunnel->dest.host_name, tunnel->dest.host_port,
			"HTTP/1.1\r\nHost:", tunnel->dest.proxy_name,
			tunnel->dest.proxy_port, "\r\n\r\n");

	n = tunnel_write(tunnel, buf, strlen(buf));
	if (n <= 0) {
		return -1;
	}

	tunnel_wait_read(tunnel->fd);
	n = tunnel_read(tunnel, buf, sizeof(buf));
	DBG("proxy recv: \n%s\n", buf);
	if (n <= 0) {
		return -1;
	}
	/* TODO: server response should be checked here */

	return 0;
}

static INLINE int do_connect(struct sockaddr_in *address)
{
	int fd;

	fd = socket(AF_INET, SOCK_STREAM, 0);
	DBG("fd: %d", fd);
	if (fd == -1) {
		DBG("create socket failed. fd: %d", fd);
		return -1;
	}

	if (connect(fd, (struct sockaddr *)address,
				sizeof(struct sockaddr_in)) == -1) {
		_socket_close(fd);
		DBG("connect socket failed. fd: %d", fd);
		return -1;
	}

	return fd;
}

int tunnel_connect(Tunnel * tunnel)
{
	if (tunnel->fd != -1) {
		DBG("tunnel already connected.");
		return 0;
	}

	tunnel->fd = do_connect(&tunnel->address);
	if (tunnel->fd == -1) {
		DBG("tunnel_in_connect: do_connect() error: %s", strerror(errno));
		return -1;
	}
#ifdef _OPENSSL_
	if (tunnel->is_https) {
		int ssl_err;
		if (SSL_set_fd(tunnel->ssl, tunnel->fd) < 1)
		{
			DBG("SSL_set_fd failed.");
			_socket_close(tunnel->fd);
			return -1;
		}
/*
		if (SSL_connect(tunnel->ssl) == -1) {
			DBG("SSL_connect failed with ssl_err: %d", ssl_err);
			_socket_close(tunnel->fd);
			return -1;
		}
*/		
		do
		{
			// SSL_WANT_READ errors are normal, just try again if it happens
			ssl_err = SSL_get_error(tunnel->ssl, SSL_connect(tunnel->ssl));
		} while (ssl_err == SSL_ERROR_WANT_READ);

		if (ssl_err != SSL_ERROR_NONE)
		{
			DBG("SSL_connect failed with ssl_err: %d", ssl_err);
			_socket_close(tunnel->fd);
			return -1;
		}
		
	}
#endif
	tunnel_send_CONNECT_request(tunnel);

	return 0;
}

void tunnel_disconnect(Tunnel * tunnel)
{
#ifdef _OPENSSL_
	if (tunnel->is_https) {
		SSL_shutdown(tunnel->ssl);
	}
#endif
	if (tunnel->fd == -1)
		return;

	_socket_close(tunnel->fd);
	tunnel->fd = -1;

	DBG("input disconnected");
}

int tunnel_wait_read(int sockfd)
{
	fd_set fds;
	if (sockfd < 1) {
		fprintf(stderr, "Invalid socket to watch: %d\n", sockfd);
		return 0;
	}
	FD_ZERO(&fds);
	FD_SET(sockfd, &fds);
	select(sockfd + 1, &fds, NULL, NULL, NULL);

	return 0;
}

int tunnel_read(Tunnel * tunnel, void *data, int length)
{
	if (tunnel->is_https) {
#ifdef _OPENSSL_
		return SSL_read(tunnel->ssl, data, length);
#else
		return -1;
#endif
	}
	else {
		return recv(tunnel->fd, data, length, 0);
	}
}

int write_all(int fd, void *data, int len)
{
	int n, m;
	char *wdata = data;

	for (n = 0; n < len; n += m) {
		m = send(fd, wdata + n, len - n, 0);
		if (m == 0)
			return 0;
		else if (m == -1) {
			if (errno != EAGAIN)
				return -1;
			else
				m = 0;
		}
	}

	return len;
}

#ifdef _OPENSSL_
int ssl_write_all(SSL *ssl, void *data, int len)
{
	int n, m;
	char *wdata = data;

	for (n = 0; n < len; n += m) {
		m = SSL_write(ssl, wdata + n, len - n);
		if (m == 0)
			return 0;
		else if (m == -1) {
			if (errno != EAGAIN)
				return -1;
			else
				m = 0;
		}
	}

	return len;
}
#endif

int tunnel_write(Tunnel * tunnel, void *data, int length)
{
	if (tunnel->is_https) {
#ifdef _OPENSSL_
		return ssl_write_all(tunnel->ssl, data, length);
#else
		return -1;
#endif
	}
	else {
		return write_all(tunnel->fd, data, length);
	}
}

