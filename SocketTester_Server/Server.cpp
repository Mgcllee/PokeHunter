#include <iostream>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsaData;
    SOCKET serverSocket, clientSocket;
    SOCKADDR_IN serverAddr, clientAddr;
    char buffer[1024];

    // Winsock 초기화
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cout << "Failed to initialize winsock." << std::endl;
        return -1;
    }

    // 소켓 생성
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        std::cout << "Failed to create socket." << std::endl;
        return -1;
    }

    // 소켓 주소 설정
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8888);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // 소켓 바인딩
    if (bind(serverSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cout << "Failed to bind socket." << std::endl;
        closesocket(serverSocket);
        return -1;
    }

    // 클라이언트 연결 대기
    listen(serverSocket, 1);
    std::cout << "Server listening on port 8888..." << std::endl;

    // 클라이언트 연결 수락
    int clientAddrSize = sizeof(clientAddr);
    if ((clientSocket = accept(serverSocket, (SOCKADDR*)&clientAddr, &clientAddrSize)) == INVALID_SOCKET) {
        std::cout << "Failed to accept client connection." << std::endl;
        closesocket(serverSocket);
        return -1;
    }
    std::cout << "Client connected." << std::endl;

    // 클라이언트로부터 데이터 수신 및 송신
    while (true) {
        memset(buffer, 0, sizeof(buffer));

        // 클라이언트로부터 데이터 수신
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead == SOCKET_ERROR) {
            std::cout << "Failed to receive data from client." << std::endl;
            break;
        }

        // 클라이언트로부터 받은 데이터 출력
        std::cout << "Received from client: " << buffer << std::endl;

        // 클라이언트로 데이터 송신
        if (send(clientSocket, buffer, bytesRead, 0) == SOCKET_ERROR) {
            std::cout << "Failed to send data to client." << std::endl;
            break;
        }
    }

    // 연결 종료 및 소켓 정리
    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
