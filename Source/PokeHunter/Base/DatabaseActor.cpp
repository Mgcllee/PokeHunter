// Fill out your copyright notice in the Description page of Project Settings.


#include "DatabaseActor.h"

// Sets default values
ADatabaseActor::ADatabaseActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADatabaseActor::BeginPlay()
{
	Super::BeginPlay();

	for (auto& ItemDataClass : ItemDataClassMap)
	{
		auto ItemData = NewObject<UItemData>(this, ItemDataClass.Value);
		ItemDataObjectMap.Add(ItemDataClass.Key, ItemData);
	}
	
}

// Called every frame
void ADatabaseActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

UItemData* ADatabaseActor::FindItem(FName ItemID)
{
	if (ItemDataObjectMap.Find(ItemID)) 
		return *ItemDataObjectMap.Find(ItemID);
	else return NULL;
}

