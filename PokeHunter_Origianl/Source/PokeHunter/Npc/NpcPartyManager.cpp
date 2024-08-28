// Fill out your copyright notice in the Description page of Project Settings.


#include "NpcPartyManager.h"
#include "PokeHunter/Hunter/Hunter.h"
#include "Blueprint/UserWidget.h"


// Sets default values
ANpcPartyManager::ANpcPartyManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.

}

// Called when the game starts or when spawned
void ANpcPartyManager::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickEnabled(0);
}

// Called every frame
void ANpcPartyManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ANpcPartyManager::Interact_Implementation(AHunter* Hunter)
{
	ANpc::Interact_Implementation(Hunter);
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ANpcPartyManager::OpenUI, 1.0f, false, 1.0f);

}