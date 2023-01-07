#pragma once
#include "stdafx.h"
#include "netModule.h"

int main() {
	WSADATA WSAData;

	int err_code;
	err_code = WSAStartup(MAKEWORD(2, 2), &WSAData);


	WSACleanup();
}
