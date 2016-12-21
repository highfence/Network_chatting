﻿#include "stdafx.h"
#define TCPServer

#ifdef TCPServer


// 소켓 함수 오류 출력 후 종료
void err_quit(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL
	);

	MessageBox(NULL, (LPCTSTR)lpMsgBuf, (LPCTSTR)msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(-1);
}

// 소켓 함수 오류 출력
void err_display(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL
	);
	printf("[%s] %s", msg, (LPCTSTR)lpMsgBuf);
	LocalFree(lpMsgBuf);
	return;
}

void clientThread(SOCKET client_sock, SOCKADDR_IN* clientaddr, std::vector<SOCKET>* clientVector)
{
	int retval;
	char buf[BUFSIZE];

	// 클라이언트와 데이터 통신
	while (true)
	{

		// 데이터 받기
		retval = recv(client_sock, buf, BUFSIZE, 0);
		if (retval == SOCKET_ERROR)
		{
			break;
		}

		else if (retval == 0) break;

		// 받은 데이터 출력
		buf[retval] = '\0';
		printf("[TCP/%s:%d] %s\n", inet_ntoa(clientaddr->sin_addr), ntohs(clientaddr->sin_port), buf);

		// 데이터 보내기
		std::vector<SOCKET>::iterator iter = clientVector->begin();
		for (; iter != clientVector->end(); ++iter)
		{
			retval = send(*iter, buf, retval, 0);
			if (retval == SOCKET_ERROR)
			{
				err_display("send()");
				break;
			}
		}
	}

	closesocket(client_sock);
	printf("[TCP 서버] 클라이언트 종료 : IP 주소 = %s, 포트 번호 = %d \n", inet_ntoa(clientaddr->sin_addr), ntohs(clientaddr->sin_port));
}

int main(int argc, char* argv[])
{
	int retval;
	std::vector<SOCKET> clientVector;

	// 윈속 초기화.
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return -1;

	// socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_quit("socket()");

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(9000);
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");
	
	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");

	// 데이터 통신에 사용할 변수
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	char buf[BUFSIZE + 1];

	while (true)
	{
		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			continue;
		}
		printf("\n[TCP 서버] 클라이언트 접속 : IP 주소 = %s, 포트 번호 = %d \n", 
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		clientVector.push_back(client_sock);
		auto th1 = std::thread(clientThread, client_sock, &clientaddr, &clientVector);
		th1.detach();
		
	}
		// close socket()
	for (auto socket : clientVector)
	{
		closesocket(socket);
		printf("[TCP 서버] 클라이언트 종료 : IP 주소 = %s, 포트 번호 = %d \n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
	}
	closesocket(listen_sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}

#endif