#pragma once
#include "stdafx.h"
#include "netModule.h"

int main() {
	WSADATA WSAData;

	int err_code;
	err_code = WSAStartup(MAKEWORD(2, 2), &WSAData);

	/*
	Overlapped I/O -> IOCP 

	IOCP = non-blocking + asynchronous
	SELECT = blocking + asynchronous

	[Overlapped I/O]
	Overlapped I/O 로 데이터를 동시에 받고
	Completion Routine 함수로 데이터를 처리한다.

	[IOCP]
	Input/Output Completion Port의 약자
	Port는 네트워크 상의 의미가 아닌 작업, 서비스 등을 처리하는 객체
	입력이 완료되면(Overlapped I/O 데이터 모두 받으면) Completion Queue에 쌓인다.
	Completion Queue에 작업이 쌓이면(생기면) Worker Thread 를 깨워 데이터 처리

	IOCP는 Overlapped I/O 모델의 확장판이기 때문에 Overlapped I/O의 구조체와 함수를 사용

	[필수 함수]
	1. CreateIoCompletionPort
	2. GetQueuedCompletionPort
	3. PostQueuedCompletionPort
	*/
	


	WSACleanup();
}
