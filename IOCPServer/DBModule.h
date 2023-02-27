#pragma once

#include "netModule.h"

int get_player_uid() {
	for (int i = 0; i < MAX_USER; ++i) {
		if (0 == strcmp(clients[i]._name, "empty")) {
			return i;
		}
	}
	return -1;
}

bool checking_DB() {

}