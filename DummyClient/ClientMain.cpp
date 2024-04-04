#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<iostream>
#include<string>
#include <winsock2.h>
#include <thread>

#include "../IOCPServer/protocol.h"

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")

using namespace std;

#define send_defualt_text "채팅 (60자 이내, 종료: none입력): "

SOCKET clientSocket;

COORD recv_text_pos = { 0, 0 };
COORD send_text_pos = { strlen(send_defualt_text), 1};

void ShowErrorMessage(string message)
{
	cout << "[오류발생]: " << message << '\n';

	cout << "ErrorCode: " << WSAGetLastError();

	system("pause");
	exit(1);
}

void update_recv_date()
{
	while (true)
	{
		CS_CHAT_TEXT_PACK ctp;
		recv(clientSocket, (char*)&ctp, sizeof(CS_CHAT_TEXT_PACK), NULL);

		if (ctp.type == CS_CHAT_TEXT) 
		{
			printf("%s\n", ctp.content);
		}
	}
}

void net_moudle()
{
	while (true)
	{
		std::string buf;
		printf("%s", send_defualt_text);
		std::cin >> buf;

		if (buf == "none") break;

		CS_CHAT_TEXT_PACK ctp;
		ctp.size = sizeof(CS_CHAT_TEXT_PACK);
		ctp.type = CS_CHAT_TEXT;
		strcpy_s(ctp.content, buf.c_str());
		send(clientSocket, (char*)&ctp, ctp.size, NULL);
	}
}

int main()
{
	string dummy_test_name{};
	std::cin >> dummy_test_name;

	WSADATA wsaData;

	char received[256];

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) // Winsock을초기화합니다.
		ShowErrorMessage("WSAStartup()");

	clientSocket = socket(PF_INET, SOCK_STREAM, 0); // TCP 소켓을생성합니다.

	if (clientSocket == INVALID_SOCKET)
		ShowErrorMessage("socket()");

	SOCKADDR_IN serverAddress;
	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
	serverAddress.sin_port = htons(PORT_NUM);

	if (connect(clientSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
		ShowErrorMessage("connect()");
	printf("[Client]\n");

	CS_LOGIN_PACK test_pack;
	test_pack.size = sizeof(CS_LOGIN_PACK);
	test_pack.type = CS_LOGIN;

	std::string dummy_name = string("dummy_client_").append(dummy_test_name);
	strncpy_s(test_pack.Token, dummy_name.c_str(), dummy_name.length());

	send(clientSocket, (char*)&test_pack, test_pack.size, NULL);

	SC_LOGIN_INFO_PACK info_pack;
	recv(clientSocket, (char*)&info_pack, sizeof(SC_LOGIN_INFO_PACK), NULL);

	printf("name: %s\n", info_pack.name);
	printf("pet: %s\n", info_pack._pet_num);
	printf("name: %c\n", info_pack._player_skin);

	CONSOLE_SCREEN_BUFFER_INFO cs;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cs);

	recv_text_pos.Y = cs.dwCursorPosition.Y + 1;

	std::thread net_module(net_moudle);
	// std::thread recv_module(update_recv_date);
	
	while (true) 
	{
		CS_CHAT_TEXT_PACK ctp;
		int ret = recv(clientSocket, (char*)&ctp, sizeof(CS_CHAT_TEXT_PACK), NULL);

		if (ret != 0) 
		{
			recv_text_pos.Y += 1;
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), recv_text_pos);

			send_text_pos.Y = recv_text_pos.Y + 1;
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), send_text_pos);

			if (ctp.type == CS_CHAT_TEXT)
			{
				printf("%s\n", ctp.content);
			}
		}
		
	}
	
	// recv_module.join();
	net_module.join();
	

	closesocket(clientSocket);
	WSACleanup();
}