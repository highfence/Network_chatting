#include "stdafx.h"
#define TCPClient

#ifdef TCPClient

std::string staticUserId;
static char chatId[BUFSIZE];
const char delimeter[] = ">";

// ���� �Լ� ���� ��� �� ����
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

// ���� �Լ� ���� ���
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

		// ���� ������ ���
		buf[retval] = '\0';
		printf("%s\n",  buf);
	}
}

int main(int argc, char* argv[])
{
	int retval;
	std::cout << "����Ͻ� ID�� �Է����ּ��� : ";
	std::cin >> staticUserId;
	staticUserId += "> ";

	// ���� �ʱ�ȭ
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

	// ������ ��ſ� ����� ����
	char buf[BUFSIZE + 1];
	int len;

	// ������ ������ ���
	while (true)
	{
		// ������ �Է�
		ZeroMemory(buf, sizeof(buf));
		printf("\n[���� ������]");

		std::string stringToSend = "";
		std::getline(std::cin, stringToSend);
		
		stringToSend = staticUserId + stringToSend;
		retval = send(sock, stringToSend.c_str(), stringToSend.length() + 1, 0);

		// ������ ������
		if (retval == SOCKET_ERROR)
		{
			err_display("send()");
			break;
		}
		printf("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� ���½��ϴ�. \n", retval);

	}

	// close socket()
	closesocket(sock);

	// ���� ����
	WSACleanup();
	return 0;
}
#endif