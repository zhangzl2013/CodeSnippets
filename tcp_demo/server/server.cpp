/**
 * TCP Server Demo
 *
 * Copyright 2014 Zhang Zhaolong <zhangzhaolong0454@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <WinSock2.h>
#include <io.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "../common/common.h"
#include "../common/protocol.h"

#pragma comment(lib, "ws2_32.lib")

int demo_send_file(SOCKET clientfd, const char *name)
{
	int ret;
	int fd;
	long file_len;
	DemoHeader header;
	char *data;
	int readlen;

	fd = _open(name, _O_RDONLY | _O_BINARY);
	if (fd < 0) {
		demo_err("open file %s failed. error: %x\n", name, GetLastError());
		return -1;
	}

	file_len =  _filelength(fd);

	demo_info("file length: %d\n", file_len);
	data = (char *)malloc(file_len);
	if (!data) {
		demo_err("insufficient memory.\n");
		return -1;
	}

	readlen = 0;
	while (readlen < file_len) {
		int len;
		len = read(fd, data + readlen, file_len - readlen);
		if (len < 0) {
			demo_err("read err.\n");
			return -1;
		}
		demo_info("read file readlen: %d. len: %d\n", readlen, len);
		readlen += len;
		break;
	}
	close(fd);

	demo_info("read file ok.\n");
	header.type = MSG_FILE;
	header.length = readlen;

	ret = send_all(clientfd, (char *)&header, sizeof(DemoHeader), 0);
	if (ret < 0) {
		demo_err("send header failed.\n");
		return -1;
	}

	ret = send_all(clientfd, data, readlen, 0);
	if (ret < 0) {
		demo_err("send header failed.\n");
		return -1;
	}

	return 0;
}

DWORD WINAPI worker(LPVOID lparam)
{
	int ret;
	int close_worker = 0;
	SOCKET clientfd = (SOCKET)lparam;
	DemoHeader header;
	char *filename = NULL;

	demo_info("enter new worker thread.\n");

	while (!close_worker) {
		ret = recv_all(clientfd, (char *)&header, sizeof(DemoHeader), 0);
		if (ret < 0) {
			return -1;
		}

		demo_info("received: type: %x, length: %d\n", header.type, header.length);

		switch (header.type) {
		case MSG_QUIT:
			demo_info("close server.\n");
			close_worker = 1;
			break;

		case MSG_QUERY:
		{
			filename = (char *)malloc(header.length);
			memset(filename, 0, header.length);

			ret = recv_all(clientfd, filename, header.length, 0);
			if (ret < 0) {
				demo_err("MSG_QUERY failed\n");
				return -1;
			}
			printf("msg_query:%s\n", filename);

			demo_send_file(clientfd, filename);

			free(filename);
			filename = NULL;

			break;
		}
		case MSG_INFO:
		{
			char *data;

			data = (char *)malloc(header.length);
			memset(data, 0, header.length);

			ret = recv_all(clientfd, data, header.length, 0);
			if (ret < 0) {
				return -1;
			}
			printf("msg_info:%s\n", data);

			free(data);

			break;
		}

		case MSG_FILE:
			demo_err("invalid type for server.\n");
			break;

		default:
			demo_err("unknown type.\n");
		}
	}

	return 0;
}

int start_server()
{
	WSADATA wsa;
	SOCKET serverfd;
	SOCKET clientfd;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	int addrlen = sizeof(struct sockaddr_in);

	if (WSAStartup(MAKEWORD(2,2),&wsa) != 0) {
		return -1;
	}

	serverfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverfd == INVALID_SOCKET) {
		return -1;
	}

	memset(&server_addr,0,sizeof(sockaddr_in));

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(1100);

	if (bind(serverfd, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
		return -1;
	}

	if (listen(serverfd, SOMAXCONN) == SOCKET_ERROR) {
		return -1;
	}

	memset(&client_addr,0,sizeof(client_addr));

	while (1)
	{
		clientfd = accept(serverfd, (sockaddr*)&client_addr, &addrlen);
		if (clientfd == INVALID_SOCKET) {
			return -1;
		} else {
			CreateThread(NULL, NULL, worker, (LPVOID)clientfd, NULL, NULL);
		}
	}

	return 0;
}

int main()
{
	start_server();

	return 0;
}