#pragma once

#include "PacketWorker.h"

void PacketWorker::worker_thread(HANDLE h_iocp) {
  iocp_handle = h_iocp;

  while (true) {
    GQCS_result = GetQueuedCompletionStatus(iocp_handle, &num_bytes, &key,
                                            &overlapped, INFINITE);
    OverlappedExpansion* exoverlapped =
        reinterpret_cast<OverlappedExpansion*>(overlapped);

    if (check_exists_overlapped(exoverlapped)) {
      continue;
    }

    switch (exoverlapped->socket_type) {
      case SOCKET_TYPE::ACCEPT:
        accept_new_client();
        break;

      case SOCKET_TYPE::RECV:
        recv_new_message(exoverlapped);
        break;

      case SOCKET_TYPE::SEND:
        delete exoverlapped;
        break;

      default:
        send_log("[Process Function]: Wrong SOCKET TYPE " +
                 std::to_string(key));
        break;
    }
  }
}

void PacketWorker::recv_new_message(OverlappedExpansion* exoverlapped) {
  int player_ticket = static_cast<int>(key);
  int remain_data = num_bytes + players->get_player(player_ticket)
                                    ->get_session()
                                    ->get_prev_remain_packet_size();
  char* p = exoverlapped->packet_buffer;

  while (remain_data > 0) {
    int packet_size = p[0];
    if (packet_size <= remain_data) {
      process_packet(static_cast<int>(player_ticket), p);
      p = p + packet_size;
      remain_data = remain_data - packet_size;
    } else
      break;
  }

  players->get_player(player_ticket)
      ->get_session()
      ->set_curr_remain_packet_size(remain_data);

  if (remain_data > 0) {
    memcpy(exoverlapped->packet_buffer, p, remain_data);
  }
  players->get_player(player_ticket)->get_session()->recv_packet();
}

void PacketWorker::process_packet(int player_ticket, char* packet) {
  switch (packet[1]) {
    case SEND_CLIENT_TO_SERVER_PACKET_TYPE::CS_LOGIN:
      players->get_player(player_ticket)->check_exists_token(packet);
      break;
    case SEND_CLIENT_TO_SERVER_PACKET_TYPE::CS_CHAT:
      sync_new_chatting_all_client(player_ticket, packet);
      break;
    case SEND_CLIENT_TO_SERVER_PACKET_TYPE::CS_QUEST_INVENTORY:
      players->get_player(player_ticket)->get_all_inventory_item();
      break;
    case SEND_CLIENT_TO_SERVER_PACKET_TYPE::CS_SAVE_INVENTORY:
      players->get_player(player_ticket)->set_inventory_item(packet);
      break;
    case SEND_CLIENT_TO_SERVER_PACKET_TYPE::CS_QUEST_STORAGE:
      players->get_player(player_ticket)->get_all_storage_item();
      break;
    case SEND_CLIENT_TO_SERVER_PACKET_TYPE::CS_PARTY_SEARCHING:
      parties->get_party_list(player_ticket);
      break;
    case SEND_CLIENT_TO_SERVER_PACKET_TYPE::CS_PARTY_ENTER:
      parties->enter_party(player_ticket, packet);
      break;
    case SEND_CLIENT_TO_SERVER_PACKET_TYPE::CS_PARTY_READY:
      players->get_player(player_ticket)->set_ready_in_party();
      break;
    case SEND_CLIENT_TO_SERVER_PACKET_TYPE::CS_PARTY_LEAVE:
      players->get_player(player_ticket)->leave_current_party();
      break;
    case SEND_CLIENT_TO_SERVER_PACKET_TYPE::CS_LOGOUT:
      players->get_player(player_ticket)->recycle_player();
      break;
  }
}

bool PacketWorker::check_exists_overlapped(OverlappedExpansion* exoverlapped) {
  if (FALSE == GQCS_result) {
    if (exoverlapped->socket_type == ACCEPT) {
      send_log("Accept error\n");
    } else {
      int player_ticket = static_cast<int>(key);
      send_log("GQCS Error on client[" + std::to_string(player_ticket) + "]\n");
      disconnect(player_ticket);
    }
  }
  if ((0 == num_bytes) && (exoverlapped->socket_type == RECV)) return false;

  return true;
}

void PacketWorker::sync_new_chatting_all_client(int player_ticket,
                                                char* packet) {
  std::string content = reinterpret_cast<CS_CHAT_PACK*>(packet)->content;

  std::string chat_log =
      std::string("[chat][")
          .append(players->get_player(player_ticket)->get_user_name())
          .append("]: ")
          .append(content);

  SC_CHAT_PACK ctp;
  ctp.size = sizeof(SC_CHAT_PACK);
  ctp.type = SC_CHAT;
  strcpy_s(ctp.content, chat_log.c_str());

  players->send_all_player(&ctp);
}

void PacketWorker::disconnect(int player_ticket) {
  players->get_player(player_ticket)->get_session()->~Session();
}

void PacketWorker::accept_new_client() {
  get_new_client_ticket();

  ZeroMemory(&accepter_overlapped.overlapped,
             sizeof(accepter_overlapped.overlapped));
  int addr_size = sizeof(SOCKADDR_IN);
  AcceptEx(server_socket, client_accept_socket,
           accepter_overlapped.packet_buffer, 0, addr_size + 16, addr_size + 16,
           0, &accepter_overlapped.overlapped);
}

int PacketWorker::get_new_client_ticket() {
  // TODO: make new client ticket
  int player_ticket = -1;

  set_new_client_ticket(player_ticket);
  return player_ticket;
}

void PacketWorker::set_new_client_ticket(int player_ticket) {
  int new_player_ticket = get_new_client_ticket();

  if (-1 != new_player_ticket) {
    players->init_player(client_accept_socket, new_player_ticket);
    CreateIoCompletionPort(reinterpret_cast<HANDLE>(client_accept_socket),
                           iocp_handle, new_player_ticket, 0);
    players->get_player(new_player_ticket)->get_session()->recv_packet();
    client_accept_socket =
        WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
  }
}

void PacketWorker::send_log(std::string log) {
  // TODO: send to log thread
}

void PacketWorker::run_packet_worker_threads(HANDLE h_iocp) {
  std::vector<std::thread> worker_threads;
  int num_threads = std::thread::hardware_concurrency();

  // TODO: make log thread use all log

  // TODO: USER_DB_MANAGER class run on db_thread

  PlayerManager* player_manager = new PlayerManager();
  PartiesManager* parties_manager = new PartiesManager(player_manager);

  for (int i = 0; i < num_threads; ++i) {
    worker_threads.emplace_back(
        &PacketWorker::worker_thread,
        new PacketWorker(player_manager, parties_manager), h_iocp);
  }

  for (auto& each_thread : worker_threads) {
    each_thread.join();
  }
}