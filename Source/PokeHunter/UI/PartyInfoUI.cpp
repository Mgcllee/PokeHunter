// Fill out your copyright notice in the Description page of Project Settings.


#include "PartyInfoUI.h"

bool UPartyInfoUI::SendClientState()	// Send this client State
{
	if (nullptr == gameinstance)				return false;
	if (0 == gameinstance->PartyListMap.Num())	return false;

	bool retVal = false;
	int32 bSize;

	CS_PARTY_READY_PACK ready_pack;
	ready_pack.size = sizeof(CS_PARTY_READY_PACK);
	ready_pack.type = CS_PARTY_READY;
	retVal = gameinstance->gSocket->Send(reinterpret_cast<const uint8*>(&ready_pack), ready_pack.size, bSize);
	if (false == retVal) return false;

	return true;
}

bool UPartyInfoUI::TickSendPartyInfo()	// Request Client -> Server
{
	if (nullptr == gameinstance)				return false;
	if (0 == gameinstance->PartyListMap.Num())	return false;
	if (-1 == SelctPartyNumber)					return false;

	// ���ο� ��Ƽ ������ ���ؼ� ������ ������ ���� �����.
	PlayerName.Empty();
	PlayerPetName.Empty();
	PartyMemberState.Empty();

	bool retVal = false;
	int32 bSize;
	
	CS_PARTY_INFO_PACK quest_pack;
	quest_pack.size = sizeof(CS_PARTY_INFO_PACK);
	quest_pack.type = CS_PARTY_INFO;
	quest_pack.party_num = SelctPartyNumber;
	retVal = gameinstance->gSocket->Send(reinterpret_cast<const uint8*>(&quest_pack), quest_pack.size, bSize);
	if (false == retVal) return false;

	SC_PARTY_INFO_PACK info_pack;
	for (int i = 0; i < 4; ++i) {	// �ִ� �÷��̾� �� ��ŭ�� �ݺ��Ѵ�.
		memset(&info_pack, 0, sizeof(SC_PARTY_INFO_PACK));
		retVal = gameinstance->gSocket->Recv(reinterpret_cast<uint8*>(&info_pack), sizeof(SC_PARTY_INFO_PACK), bSize);

		// ����/���� ������ ū ������ �˻�
		if (false == retVal)									return false;	// ���� ����� ����
		if (0 == strcmp(info_pack._mem, "theEnd"))				break;			// ��Ƽ��� ���� ����
		if (i == 0 && (0 == strcmp(info_pack._mem, "Empty")))	break;			// ù ��° ������� �������
		if (i > 0 && (0 == strcmp(info_pack._mem, "Empty")))	break;			// �� ��° ��� ���� �������

		TCHAR MBTWBuffer[128];
		memset(MBTWBuffer, NULL, 128);
		MultiByteToWideChar(CP_ACP, 0, (LPCSTR)info_pack._mem, -1, MBTWBuffer, strlen(info_pack._mem));
		
		// ���� ��Ƽ ����� �غ���� ǥ�ÿ�
		PlayerName.Add(FName(MBTWBuffer));
		PlayerPetName.Add(PLAYER_PET(info_pack._mem_pet));		// �۵��� �ȵ� ���, TArray�� �ڷ����� ����
		PartyMemberState.Add(PLAYER_STATE(info_pack._mem_state));
	}

	return true;
}

bool UPartyInfoUI::RecvClientJoin()	// [Tick���� Call!!!] Client���� AWS GameLift�� �����϶�� ��ȣ�� ����� ����
{
	if (nullptr == gameinstance)	return false;
	if (-1 == SelctPartyNumber)		return false;
	bool retVal = false;
	int32 bSize;

	SC_PARTY_JOIN_SUCCESS_PACK start_party_pack;
	retVal = gameinstance->gSocket->Recv(reinterpret_cast<uint8*>(&start_party_pack), sizeof(SC_PARTY_JOIN_SUCCESS_PACK), bSize);

	if (true == retVal) {
		// AWS ����
		return true;
	}
	else				return false;

	// Clinet���� ��� ���� ���
	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("[Recv] IOCP server...")));

	return true;
}
