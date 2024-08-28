// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryServerManager.h"
#include "PokeHunter/Hunter/Hunter.h"
#include "PokeHunter/Hunter/InventoryComponent.h"

UInventoryServerManager::UInventoryServerManager()
{
	// gameinstance = Cast<UBaseInstance>(UGameplayStatics::GetGameInstance((GetWorld())));
}

bool UInventoryServerManager::GetInvenInfo(ACharacter* myPlayer, UBaseInstance* gameinstance)
{
	if (nullptr == gameinstance->gSocket) return false;
	if (ESocketConnectionState::SCS_NotConnected == gameinstance->gSocket->GetConnectionState()) return false;
	if (ESocketConnectionState::SCS_ConnectionError == gameinstance->gSocket->GetConnectionState()) return false;

	int32 bSize{};
	CS_QUEST_INVENTORY_PACK quest_item;
	quest_item.size = sizeof(CS_QUEST_INVENTORY_PACK);
	quest_item.type = CS_QUEST_INVENTORY;
	gameinstance->gSocket->Send(reinterpret_cast<const uint8*>(&quest_item), quest_item.size, bSize);

	SC_ITEM_INFO_PACK item_info{};
	int index = 0;
	for ( ; ; ++index) {
		memset(&item_info, 0, sizeof(SC_ITEM_INFO_PACK));
		gameinstance->gSocket->Recv(reinterpret_cast<uint8*>(&item_info), sizeof(SC_ITEM_INFO_PACK), bSize);
		FName msg_name = item_info._name;
		int msg_cnt = item_info._cnt;
		if (msg_name == "theEnd") break;

		gameinstance->InfoArray.Add(FItemCnter{ msg_name, msg_cnt });
	}

	UE_LOG(LogTemp, Warning, TEXT("gameinstance->InfoArray.Num() : %d"), gameinstance->InfoArray.Num());
	return true;
}

bool UInventoryServerManager::SaveInventoryDB(UBaseInstance* gameinstance)
{
	if (ESocketConnectionState::SCS_NotConnected == gameinstance->gSocket->GetConnectionState()) return false;
	if (ESocketConnectionState::SCS_ConnectionError == gameinstance->gSocket->GetConnectionState()) return false;

	CS_SAVE_INVENTORY_PACK save_pack;
	save_pack.size = sizeof(CS_SAVE_INVENTORY_PACK);
	save_pack.type = CS_SAVE_INVENTORY;

	AHunter* MyHunter = Cast<AHunter>(UGameplayStatics::GetPlayerCharacter(this, 0));

	for (FItemCnter& item : MyHunter->Inventory->InfoArray) {
		strncpy_s(save_pack._name, CHAR_SIZE, TCHAR_TO_ANSI(*item.ItemID.ToString()), strlen(TCHAR_TO_ANSI(*item.ItemID.ToString())));
		save_pack._cnt = item.cnt;
		// send(gameinstance->gSocket, (char*)&save_pack, save_pack.size, NULL);
	}

	strncpy_s(save_pack._name, CHAR_SIZE, "theEnd", strlen("theEnd"));
	// send(gameinstance->gSocket, (char*)&save_pack, save_pack.size, NULL);
	return true;
}
