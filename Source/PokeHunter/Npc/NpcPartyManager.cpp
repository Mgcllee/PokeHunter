// Fill out your copyright notice in the Description page of Project Settings.


#include "NpcPartyManager.h"
#include "PokeHunter/Hunter/Hunter.h"
#include "Blueprint/UserWidget.h"


// Sets default values
ANpcPartyManager::ANpcPartyManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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

void ANpcPartyManager::interact_Implementation(AHunter* Hunter)
{
	Super::interact_Implementation(Hunter);
	if (bActive)
	{
		GetWorldTimerManager().SetTimer(TimerHandle, this, &ANpcPartyManager::OpenUI, 1.0f, false, 1.0f);
	}

}

void ANpcPartyManager::OpenUI()
{
	if (bActive)
	{
		bActive = 0;
	}
	else
	{
		if (Master->StorageUI == nullptr)
		{
			Master->StorageUI = CreateWidget(GetWorld(), Master->StorageUIClass);
		}
		Master->StorageUI->AddToViewport();
		bActive = 1;
	}
	Master->EnableInput(Cast<APlayerController>(Master->Controller));
	GetWorldTimerManager().ClearTimer(TimerHandle);
}