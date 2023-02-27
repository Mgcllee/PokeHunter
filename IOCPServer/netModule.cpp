#include "netModule.h"

void worker_thread(HANDLE h_iocp)
{
	while (true) {
		DWORD num_bytes;
		ULONG_PTR key;
		WSAOVERLAPPED* over = nullptr;
		BOOL ret = GetQueuedCompletionStatus(h_iocp, &num_bytes, &key, &over, INFINITE);
		OVER_EXP* ex_over = reinterpret_cast<OVER_EXP*>(over);

		// error 검출기
		if (FALSE == ret) {
			if (ex_over->c_type == ACCEPT) {
				std::string err_text = "errorCollecter.txt";
				std::ofstream err_file(err_text.data());

				if (err_file.is_open()) {
					err_file << "";

					time_t timer;
					struct tm* t;

					timer = time(NULL);
					t = localtime(&timer);

					err_file << t->tm_year + 1900 << "/" << t->tm_mon + 1 << "/" << t->tm_mday
						<< "-(" << t->tm_hour << ":" << t->tm_min << ":" << t->tm_sec << ")"
						<< " : Accept Error\n";
				}
			}
			else continue;
		}
		if ((0 == num_bytes) && (ex_over->c_type == RECV)) continue;

		switch (ex_over->c_type) {
		case ACCEPT:	// accept new client
		{
			// newClient 고유번호 부여, 게임정보 입력, 연결
		}
		break;
		case RECV:		// get new message
		{
			// 패킷 재조립
		}
		break;
		case SEND:		// send new message
		{

		}
		break;
		}
	}
}

void process_packet()
{

}
