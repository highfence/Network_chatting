#include "stdafx.h"
#define TCPClient

#ifdef TCPClient

std::string staticUserId;
static char chatId[BUFSIZE];
const char delimeter[] = ">";

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

void recvFunc(SOCKET sock)
{
	int retval = 0;
	char buf[BUFSIZE + 1];
	while (1)
	{
		retval = recv(sock, buf, BUFSIZE, 0);
		if (retval == SOCKET_ERROR)
		{
			int error = WSAGetLastError();
			err_display("recv()");
			break;
		}

		else if (retval == 0) break;

		// 받은 데이터 출력
		buf[retval] = '\0';
		printf("%s\n",  buf);
	}
}

int main(int argc, char* argv[])
{
	int retval;
	std::cout << "사용하실 ID를 입력해주세요 : ";
	std::cin >> staticUserId;
	staticUserId += "> ";

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return -1;

	// socket()
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");



	// connect()
	SOCKADDR_IN serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(9000);
	serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("connect()");

	// TODO :: recvFunc thread
	auto recvThread = std::thread(recvFunc, sock);
	recvThread.detach();

	// 데이터 통신에 사용할 변수
	char buf[BUFSIZE + 1];
	int len;

	// 서버와 데이터 통신
	while (true)
	{
		// 데이터 입력
		ZeroMemory(buf, sizeof(buf));
		printf("\n[보낼 데이터]");

		std::string stringToSend = "";
		std::getline(std::cin, stringToSend);
		
		stringToSend = staticUserId + stringToSend;
		retval = send(sock, stringToSend.c_str(), stringToSend.length() + 1, 0);

		// 데이터 보내기
		if (retval == SOCKET_ERROR)
		{
			err_display("send()");
			break;
		}
		printf("[TCP 클라이언트] %d바이트를 보냈습니다. \n", retval);

	}

	// close socket()
	closesocket(sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}
#endif