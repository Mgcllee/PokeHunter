// Fill out your copyright notice in the Description page of Project Settings.

/*
#include "LoginServerManager.h"

ULoginServerManager::ULoginServerManager()
{
	gameinstance = Cast<UServerIntance>(UGameplayStatics::GetGameInstance((GetWorld())));
}

FU_SC_LOGIN_INFO_PACK ULoginServerManager::LoginToServer(FString in_id, FString in_pw)
{
	FU_SC_LOGIN_INFO_PACK u_info_pack;
	SC_LOGIN_INFO_PACK info_pack;
	
	CS_LOGIN_PACK msg{};
	msg.size = sizeof(CS_LOGIN_PACK);
	msg.type = CS_LOGIN;
	strcpy(msg.id, TCHAR_TO_ANSI(*in_id));
	strcpy(msg.pw, TCHAR_TO_ANSI(*in_pw));
	send(gameinstance->Socket, (char*)&msg, msg.size, NULL);
	
	SC_LOGIN_SUCCESS_PACK ok_pack{};
	recv(gameinstance->Socket, (char*)&ok_pack, sizeof(SC_LOGIN_SUCCESS_PACK), NULL);
	if (SC_LOGIN_SUCCESS == ok_pack.type) {
		recv(gameinstance->Socket, (char*)&info_pack, sizeof(SC_LOGIN_INFO_PACK), NULL);

		u_info_pack._name = ANSI_TO_TCHAR(info_pack.name);
		u_info_pack._player_skin = FString::Printf(TEXT("%hs"), info_pack._player_skin);
		u_info_pack._pet_num = FString::Printf(TEXT("%hs"), info_pack._player_skin);
		u_info_pack._q_item = FString::Printf(TEXT("%hs"), info_pack._q_item);
		u_info_pack._q_skill = ANSI_TO_TCHAR(info_pack._q_skill);

		return u_info_pack;
	}
	
	u_info_pack._name = ANSI_TO_TCHAR("empty");
	return u_info_pack;
}
*/