#include <iostream>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsaData;
    SOCKET clientSocket;
    SOCKADDR_IN serverAddr;
    char buffer[1024];

    // Winsock 초기화
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cout << "Failed to initialize winsock." << std::endl;
        return -1;
    }

    // 소켓 생성
    if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        std::cout << "Failed to create socket." << std::endl;
        return -1;
    }

    // 서버 주소 설정
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8888);
    serverAddr.sin_addr.s_addr = inet_addr("172.31.176.1"); // 서버 IP 주소

    // 서버에 연결
    if (connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cout << "Failed to connect to server." << std::endl;
        closesocket(clientSocket);
        return -1;
    }
    std::cout << "Connected to server." << std::endl;

    // 서버로부터 데이터 송신 및 수신
    while (true) {
        std::cout << "Enter message: ";
        std::cin.getline(buffer, sizeof(buffer));

        // 서버로 데이터 송신
        if (send(clientSocket, buffer, strlen(buffer), 0) == SOCKET_ERROR) {
            std::cout << "Failed to send data to server." << std::endl;
            break;
        }

        // 서버로부터 데이터 수신
        memset(buffer, 0, sizeof(buffer));
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead == SOCKET_ERROR) {
            std::cout << "Failed to receive data from server." << std::endl;
            break;
        }

        // 서버로부터 받은 데이터 출력
        std::cout << "Received from server: " << buffer << std::endl;
    }

    // 연결 종료 및 소켓 정리
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
