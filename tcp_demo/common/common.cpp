/**
 * Demo common routines
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

#include "common.h"

#pragma comment(lib, "ws2_32.lib")

/* end of protocol */
int send_all(SOCKET sockfd, const char *data, int len, int flags)
{
	int n = 0;
	int res;

	while (n < len) {
		res = send(sockfd, data + n, len - n, flags);
		if (res < 0) {
			demo_err("send failed. errno: %x\n", GetLastError());
			return -1;
		}

		n += res;
	}

	return 0;
}

int recv_all(SOCKET sockfd, char *data, int len, int flags)
{
	int n = 0;
	int res;

	while (n < len) {
		res = recv(sockfd, data + n, len - n, flags);
		if (res < 0) {
			demo_err("recv failed. errno: %x\n", GetLastError());
			return -1;
		}

		n += res;
	}

	return 0;
}