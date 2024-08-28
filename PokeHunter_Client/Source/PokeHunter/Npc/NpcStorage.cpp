// Fill out your copyright notice in the Description page of Project Settings.


#include "NpcStorage.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Blueprint/UserWidget.h"
#include "PokeHunter/Item/Item.h"
#include "PokeHunter/Item/Item.h"
#include "PokeHunter/Hunter/InventoryComponent.h"
#include "PokeHunter/Hunter/Hunter.h"

// Sets default values
ANpcStorage::ANpcStorage()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.

	//인벤토리
	Storage = CreateDefaultSubobject<UInventoryComponent>(TEXT("Storage"));
	Storage->capacity = 28;

	InteractionSphere->SetSphereRadius(100.f);
}

// Called when the game starts or when spawned
void ANpcStorage::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickEnabled(0);
}

// Called every frame
void ANpcStorage::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

void ANpcStorage::Interact_Implementation(AHunter* Hunter)
{
	ANpc::Interact_Implementation(Hunter);
}

bool ANpcStorage::GetNpcStorageInfo(UBaseInstance* gameinstance)
{
	if (nullptr == gameinstance->gSocket)															return false;
	if (ESocketConnectionState::SCS_NotConnected == gameinstance->gSocket->GetConnectionState())	return false;
	if (ESocketConnectionState::SCS_ConnectionError == gameinstance->gSocket->GetConnectionState()) return false;

	bool retVal = false;
	int32 bSize{};
	CS_QUEST_STORAGE_PACK quest_item;
	quest_item.size = sizeof(CS_QUEST_STORAGE_PACK);
	quest_item.type = CS_QUEST_STORAGE;
	retVal = gameinstance->gSocket->Send(reinterpret_cast<const uint8*>(&quest_item), quest_item.size, bSize);
	if (false == retVal) return false;

	if (false == Storage->InfoArray.IsEmpty()) {
		Storage->InfoArray.Empty();
	}

	if (true == gameinstance->StorageInfoArray.IsEmpty()) {
		SC_ITEM_INFO_PACK item_info{};
		int curCap = 0;

		for (int i = 0; ; ++i, ++curCap) {
			memset(&item_info, 0, sizeof(SC_ITEM_INFO_PACK));
			retVal = gameinstance->gSocket->Recv(reinterpret_cast<uint8*>(&item_info), sizeof(SC_ITEM_INFO_PACK), bSize);
			if (false == retVal) return false;

			FName msg_name = item_info._name;
			int msg_cnt = item_info._cnt;
			if (msg_name == "theEnd") break;

			gameinstance->StorageInfoArray.Add(FItemCnter{ msg_name, msg_cnt });
		}

		for (; curCap < Storage->capacity; ++curCap) {
			gameinstance->StorageInfoArray.Add(FItemCnter{ "None", 0 });
		}
	}

	Storage->InfoArray = gameinstance->StorageInfoArray;

	return true;
}
