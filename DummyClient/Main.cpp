#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment (lib, "Ws2_32.lib")

using namespace std;

int main() {
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo* result = NULL,
        * ptr = NULL,
        hints;

    // Winsock 초기화
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        // 초기화 실패
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // 서버 주소 정보 가져오기
    iResult = getaddrinfo("localhost", "27015", &hints, &result);
    if (iResult != 0) {
        // 주소 정보 가져오기 실패
    }

    // 서버에 연결
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

        // 소켓 생성
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            // 소켓 생성 실패
            continue;
        }

        // 서버에 연결
        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            // 연결 실패
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    // 주소 정보 메모리 해제
    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        // 서버 연결 실패
    }
}