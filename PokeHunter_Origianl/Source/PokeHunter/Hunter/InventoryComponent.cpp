// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"
#include "PokeHunter/Item/Item.h"
#include "PokeHunter/Npc//NpcStorage.h"
#include "Hunter.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	bEditableWhenInherited = true;
	// ...

	InfoArray.Reserve(capacity);
	// FarmingArray.Reserve(capacity);
}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	//아이템 배열(클래스) 초기화
	Super::BeginPlay();
	int ArrayLength;
	if (true == InfoArray.IsEmpty())	ArrayLength = 0;
	else								ArrayLength = InfoArray.Num();

	for (int i = ArrayLength; i < capacity; i++)
	{
		//아이템 struct
		InfoArray.AddDefaulted();
		FarmingArray.AddDefaulted();
	}

}

bool UInventoryComponent::AddItemData(FItemCnter ItemCnter)
{
	int NullNum = -1;
	FName ItemName = ItemCnter.ItemID;
	int ItemCnt = ItemCnter.cnt;

	for (int i = 0; i < capacity; ++i)
	{
		if (NullNum == -1 && InfoArray[i].ItemID == FName("None")) 
		{
			NullNum = i;
		}
		else if (InfoArray[i].ItemID != FName("None"))
		{
			if (InfoArray[i].ItemID == ItemName) 
			{
				//Add ItemCnt Update need
				InfoArray[i].cnt += ItemCnt;

				/*if (FarmingArray[i].ItemID == FName("None")) {
					FarmingArray[i].ItemID = InfoArray[i].ItemID;
				}
				FarmingArray[i].cnt += ItemCnt;*/

				for (auto& item : FarmingArray) {
					if (item.ItemID == ItemName) {
						item.cnt += ItemCnt;
						return true;
					}
				}
				for (auto& item : FarmingArray) {
					if (item.ItemID == FName("None")) {
						item = ItemCnter;
						return true;
					}
				}

				return true;
			}
		}
	}

	if (NullNum != -1)
	{
		//Find Object Need
		InfoArray[NullNum].ItemID = ItemName;
		InfoArray[NullNum].cnt += ItemCnt;

		for (auto& item : FarmingArray) {
			if (item.ItemID == ItemName) {
				item.cnt += ItemCnt;
				return true;
			}
		}
		for (auto& item : FarmingArray) {
			if (item.ItemID == FName("None")) {
				item = ItemCnter;
				return true;
			}
		}

		return true;
	}

	return false;
}

// 사용처 없음
bool UInventoryComponent::AddItemInfo(FName ItemID, int Cnt)
{
	int NullNum = -1;
	for (int i = 0; i < capacity; ++i)
	{
		if (NullNum == -1 && InfoArray[i].ItemID.IsNone()) NullNum = i;
		else if (!InfoArray[i].ItemID.IsNone())
		{
			if (InfoArray[i].ItemID == ItemID)
			{
				InfoArray[i].cnt += Cnt;
				// FarmingArray[i].cnt += Cnt;
				return true;
			}
		}
	}

	if (NullNum != -1)
	{
		InfoArray[NullNum].ItemID = ItemID;
		InfoArray[NullNum].cnt = Cnt;

		// FarmingArray[NullNum].ItemID = ItemID;
		// FarmingArray[NullNum].cnt = Cnt;
		return true;
	}

	return false;
}


void UInventoryComponent::ChangeSlot(AActor* Storage, FName TargetName, int TargetIndex, FName GoalName, int GoalIndex)
{
	ANpcStorage* StorageNpc;
	FItemCnter temp;
	if (TargetName == "Inventory")
	{
		temp = InfoArray[TargetIndex];
		if (GoalName == "Inventory")
		{
			InfoArray[TargetIndex] = InfoArray[GoalIndex];
			InfoArray[GoalIndex] = temp;
		}
		else 
		{
			StorageNpc = Cast<ANpcStorage>(Storage);
			if (StorageNpc)
			{
				InfoArray[TargetIndex] = StorageNpc->Storage->InfoArray[GoalIndex];
				StorageNpc->Storage->InfoArray[GoalIndex] = temp;
			}
		}
	}

	else if (TargetName == "Storage")
	{
		StorageNpc = Cast<ANpcStorage>(Storage);

		if (StorageNpc)
		{
			temp = StorageNpc->Storage->InfoArray[TargetIndex];

			if (GoalName == "Inventory")
			{
				StorageNpc->Storage->InfoArray[TargetIndex] = InfoArray[GoalIndex];
				InfoArray[GoalIndex] = temp;
				UE_LOG(LogTemp, Warning, TEXT("\n\n[StorageNpc info]\nName: %s\nCnt: %d\n\n"), *temp.ItemID.ToString(), temp.cnt);
				UE_LOG(LogTemp, Warning, TEXT("\n\n[InfoArray info]\nName: %s\nCnt: %d\n\n"), *InfoArray[GoalIndex].ItemID.ToString(), InfoArray[GoalIndex].cnt);
				UE_LOG(LogTemp, Warning, TEXT("\n\n[Index]\nTargetIndex: %d\nGoalIndex: %d\n\n"), TargetIndex, GoalIndex);
			}
			else
			{
				StorageNpc->Storage->InfoArray[TargetIndex] = StorageNpc->Storage->InfoArray[GoalIndex];
				StorageNpc->Storage->InfoArray[GoalIndex] = temp;
			}
		}
	}

}

void UInventoryComponent::SwapSlot(int TargetIndex, int GoalIndex)
{
	FItemCnter Temp = InfoArray[TargetIndex];
	InfoArray[TargetIndex] = InfoArray[GoalIndex];
	InfoArray[GoalIndex] = Temp;
}

int UInventoryComponent::GetItemCnt(FName id)
{
	int cnt = 0;
	for (auto cnter : InfoArray)
	{
		if (cnter.ItemID == id)
		{
			cnt += cnter.cnt;
		}
	}
	return cnt;
}


// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

