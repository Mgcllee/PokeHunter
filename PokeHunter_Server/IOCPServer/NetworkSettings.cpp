#pragma once

#include "NetworkSettings.h"

NetworkSettings::NetworkSettings(ULONG open_addr, USHORT open_port)
    : error_code(-1),
      h_iocp(nullptr),
      server_socket(NULL),
      client_socket(NULL) {
  error_code = WSAStartup(MAKEWORD(2, 2), &WSAData);

  server_socket =
      WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT_NUM);
  server_addr.sin_addr.S_un.S_addr = INADDR_ANY;

  bind(server_socket, reinterpret_cast<sockaddr*>(&server_addr),
       sizeof(server_addr));
  listen(server_socket, SOMAXCONN);
}

NetworkSettings::~NetworkSettings() {
  closesocket(server_socket);
  WSACleanup();
}

void NetworkSettings::create_iocp() {
  h_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
  CreateIoCompletionPort(reinterpret_cast<HANDLE>(server_socket), h_iocp, 0, 0);

  client_socket =
      WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
  accept_overlapped_expansion.socket_type = ACCEPT;
}

void NetworkSettings::accept_ex_iocp() {
  int addr_size = sizeof(client_addr);

  AcceptEx(server_socket, client_socket,
           accept_overlapped_expansion.packet_buffer, 0, addr_size + 16,
           addr_size + 16, 0, &accept_overlapped_expansion.overlapped);
}