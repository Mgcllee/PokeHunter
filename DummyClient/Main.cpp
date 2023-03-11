#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<iostream>
#include<string>
#include<winsock2.h>

#include "../IOCPServer/protocol.h"

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")

using namespace std;

void ShowErrorMessage(string message)
{
	cout << "[오류발생]: " << message << '\n';
	system("pause");
	exit(1);
}

int main()
{
	WSADATA wsaData;
	SOCKET clientSocket;
	SOCKADDR_IN serverAddress;
	
	char received[256];

	string sent;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) // Winsock을초기화합니다.
		ShowErrorMessage("WSAStartup()");

	clientSocket = socket(PF_INET, SOCK_STREAM, 0); // TCP 소켓을생성합니다.

	if (clientSocket == INVALID_SOCKET)
		ShowErrorMessage("socket()");

	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1"); // 문자열IP를네트워크바이트형식으로
	serverAddress.sin_port = htons(PORT_NUM); // 2바이트정수네트워크바이트형식으로

	if (connect(clientSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
		ShowErrorMessage("connect()");

	while (1) {
		CS_LOGIN_PACK msg;
		msg.size = sizeof(CS_LOGIN_PACK);
		msg.type = CS_LOGIN;

		cout << "ID: ";
		getline(cin, sent);
		if (sent == "") continue;	// + 문자입력 길이제한 필요
		for (int i = 0; i < CHAR_SIZE; ++i) {
			if (sent[i] == NULL) {
				msg.id[i] = '\0';
				break;
			}
			else
				msg.id[i] = sent[i];
		}

		sent.clear();
		cout << "PW: ";
		getline(cin, sent);
		if (sent == "") continue;	// + 문자입력 길이제한 필요
		for (int i = 0; i < CHAR_SIZE; ++i) {
			if (sent[i] == NULL) {
				msg.pw[i] = '\0';
				break;
			}
			else
				msg.pw[i] = sent[i];
		}

		send(clientSocket, (char*)&msg, msg.size, NULL);
		
		SC_LOGIN_INFO_PACK info_pack;
		recv(clientSocket, (char*)&info_pack, sizeof(SC_LOGIN_INFO_PACK), NULL);

		cout << "Login Success! Name: " << info_pack.name << endl << "Skin: " << info_pack._player_skin << "		Pet: " << info_pack._pet_num <<
			"	Item: " << info_pack.q_item << "	Skill: " << info_pack.q_skill << endl;
	}
	closesocket(clientSocket);
	WSACleanup();
	system("pause");
}