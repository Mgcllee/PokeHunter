#pragma once

#include <atomic>

#include <WS2tcpip.h>
#include <MSWSock.h>

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")

#include "stdafx.h"
#include "protocol.h"

enum SOCKET_TYPE { ACCEPT, RECV, SEND, LOGOUT };
enum SESSION_TYPE { FREE, ALLOC };
enum PLAYER_STATE { HOME, NOT_READY, READY, STAGE };

class OverlappedExpansion {
 public:
  OverlappedExpansion();
  OverlappedExpansion(char* packet);
  ~OverlappedExpansion();

 public:
  WSAOVERLAPPED overlapped;
  SOCKET_TYPE socket_type;
  WSABUF wsa_buffer;
  char packet_buffer[BUF_SIZE];
};

class Session : public OverlappedExpansion {
 public:
  Session();
  ~Session();

  Session& operator=(const Session& ref);

  void set_socket(SOCKET new_socket);
  void recv_packet();
  void send_packet(void* packet);

  int get_prev_remain_packet_size();
  void set_curr_remain_packet_size(int in);

  void disconnect();

  bool recycle_session();

 private:
  SOCKET socket;
  int remain_packet_size;
};
