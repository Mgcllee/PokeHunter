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

	//�κ��丮
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
