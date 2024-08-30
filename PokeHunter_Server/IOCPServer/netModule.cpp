#pragma once

#include "DBModule.h"
#include "AWSModule.h"

OverlappedExpansion::OverlappedExpansion()
	: socket_type(SOCKET_TYPE::RECV)
	, wsa_buffer(WSABUF(BUF_SIZE, packet_buffer))
{
	ZeroMemory(&overlapped, sizeof(overlapped));
}

OverlappedExpansion::OverlappedExpansion(char* packet) 
	: socket_type(SOCKET_TYPE::SEND)
	, wsa_buffer(WSABUF(packet[0], packet_buffer))
{
	ZeroMemory(&overlapped, sizeof(overlapped));
	memcpy(packet_buffer, packet, packet[0]);
}

OverlappedExpansion::~OverlappedExpansion()
{

}

Session::Session()
	: remain_packet_size(0)
	, socket(NULL)
{
	
}
Session::~Session()
{
	closesocket(socket);
}

bool Session::recycle_session()
{
	return false;
}

Session& Session::operator=(const Session& ref)
{
	this->overlapped = ref.overlapped;
	this->socket_type = ref.socket_type;
	this->wsa_buffer = ref.wsa_buffer;
	
	strncpy_s(this->packet_buffer, ref.packet_buffer, BUF_SIZE);

	this->socket = ref.socket;
	this->remain_packet_size = ref.remain_packet_size;

	return *this;
}

void Session::set_socket(SOCKET new_socket)
{
	socket = new_socket;

}

int Session::get_prev_remain_packet_size()
{
	return remain_packet_size;
}

void Session::set_curr_remain_packet_size(int curr_remain_packet_size)
{
	remain_packet_size = curr_remain_packet_size;
}

void Session::recv_packet()
{
	DWORD recv_flag = 0;
	memset(&overlapped, 0, sizeof(overlapped));
	wsa_buffer.len = BUF_SIZE - remain_packet_size;
	wsa_buffer.buf = packet_buffer + remain_packet_size;
	socket_type = RECV;
	WSARecv(socket, &wsa_buffer, 1, 0, &recv_flag, &overlapped, 0);
}

void Session::send_packet(void* packet)
{
	if (packet == nullptr || socket == NULL) return;

	OverlappedExpansion* sendoverlapped = new OverlappedExpansion{ reinterpret_cast<char*>(packet) };
	WSASend(socket, &sendoverlapped->wsa_buffer, 1, 0, 0, &sendoverlapped->overlapped, 0);
}

void Session::disconnect()
{
	closesocket(socket);
}