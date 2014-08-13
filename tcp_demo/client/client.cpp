/**
 * TCP Client Demo
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

int demo_send_query(SOCKET serverfd, char *filename)
{
	DemoHeader header;

	demo_info("demo_send_query\n");
	header.type = MSG_QUERY;
	header.length = strlen(filename) + 1;;

	send_all(serverfd, (char *)&header, sizeof(DemoHeader), 0);
	send_all(serverfd, filename, header.length, 0);
	return 0;
}

int demo_send_quit(SOCKET serverfd)
{
	DemoHeader header;

	demo_info("demo_send_quit\n");
	header.type = MSG_QUIT;
	header.length = 0;

	return send_all(serverfd, (char *)&header, sizeof(DemoHeader), 0);
}

int demo_send_info(SOCKET serverfd)
{
	DemoHeader header;

	demo_info("demo_send_info\n");
	header.type = MSG_INFO;
	header.length = strlen("abcdefgh") + 1;

	send_all(serverfd, (char *)&header, sizeof(DemoHeader), 0);
	send_all(serverfd, "abcdefgh", header.length, 0);

	return 0;
}

int save_to_file(char *data, int size)
{
	int fd;
	int len;

	fd = _open("out.txt", _O_CREAT | _O_RDWR | _O_BINARY);
	if (fd < 0) {
		demo_err("open file out.txt failed. error: %x\n", GetLastError());
		return -1;
	}

	len = write(fd, data, size);
	if (len < size) {
		demo_err("write %d bytes.\n", len);
		return -1;
	}

	close(fd);

	return 0;
}

int demo_get_file(SOCKET serverfd)
{
	int ret;
	DemoHeader header;
	char *data;

	demo_send_query(serverfd, "test.txt");

	recv_all(serverfd, (char *)&header, sizeof(DemoHeader), 0);

	if (header.type != MSG_FILE) {
		demo_err("error.\n");
		return -1;
	}

	demo_info("file length: %d\n", header.length);

	data = (char *)malloc(header.length);
	if (!data) {
		demo_err("insufficient memory.\n");
		return -1;
	}
	ret = recv_all(serverfd, data, header.length, 0);
	if (ret < 0) {
		demo_err("error.\n");
		return -1;
	}

	save_to_file(data, header.length);
	free(data);
	demo_info("receive file succeed.\n");
	return 0;
}

int start_client()
{
	unsigned long ip;
	WSADATA wsa;
	SOCKET sock;
	struct sockaddr_in server_addr;

	if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
		return -1;
	}

	if ((ip = inet_addr("127.0.0.1")) == INADDR_NONE) {
		return -1;
	}

	if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
		return -1;
	}

	memset(&server_addr, 0, sizeof(struct sockaddr_in));

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.S_un.S_addr = ip;
	server_addr.sin_port = htons(1100);

	if (connect(sock, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
		return -1;
	}

	demo_send_info(sock);
	demo_get_file(sock);
	demo_send_quit(sock);
	Sleep(5000);

	WSACleanup();

	return 0;
}

int main()
{
	return start_client();
}