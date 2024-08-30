#pragma once

#include "NetworkSettings.h"
#include "PacketWorker.h"

int main() {
	NetworkSettings* net_settings = new NetworkSettings(INADDR_ANY, PORT_NUM);
	net_settings->create_iocp();
	net_settings->accept_ex_iocp();

	HANDLE h_iocp = net_settings->get_iocp_handle();
	PacketWorker* packet_worker = new PacketWorker();
	packet_worker->run_packet_worker_threads(h_iocp);

	return 0;
}
