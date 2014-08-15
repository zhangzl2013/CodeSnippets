/**
 * @file	tunnel.h
 * @brief	Http tunnel
 * @author	Zhang Zhaolong <zhangzl2013@126.com>
 * @data	2013-07-24
 *
 */

#ifndef _TUNNEL_H_
#define _TUNNEL_H_

#ifdef _OPENSSL_
#include <openssl/ssl.h>
#include <openssl/err.h>
#endif

typedef struct {
	const char *host_name;
	int host_port;
	const char *proxy_name;
	int proxy_port;
	const char *user_agent;
} Http_destination;

typedef struct {
	int fd;
	Http_destination dest;
	int is_https;
	struct sockaddr_in address;
#ifdef _OPENSSL_
	SSL_CTX *ssl_ctx;
	SSL *ssl;
#endif
} Tunnel;

Tunnel *tunnel_new(const char *host, int host_port,
				   const char *proxy, int proxy_port, int is_https);
void tunnel_delete(Tunnel * tunnel);
int tunnel_connect(Tunnel * tunnel);
void tunnel_disconnect(Tunnel * tunnel);
void tunnel_destroy(Tunnel * tunnel);
int tunnel_wait_read(int sockfd);
int tunnel_read(Tunnel * tunnel, void *data, int length);
int tunnel_write(Tunnel * tunnel, void *data, int length);

int write_all(int fd, void *data, int len);

static INLINE void _socket_close(int fd)
{
#ifdef _WIN32_
	closesocket(fd);
#else
	close(fd);
#endif
}

#endif //_TUNNEL_H_
