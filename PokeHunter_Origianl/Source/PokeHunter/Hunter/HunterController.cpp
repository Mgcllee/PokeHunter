// Fill out your copyright notice in the Description page of Project Settings.

#include "HunterController.h"
#include <Net/UnrealNetwork.h>
#include <PokeHunter/Base/BaseInstance.h>
#include <PokeHunter/Hunter/Hunter.h>
#include <PokeHunter/Base/HunterState.h>

void AHunterController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AHunterController::BeginPlay()
{
	Super::BeginPlay();

	PlayerCameraManager->ViewPitchMax = 45.f;
	PlayerCameraManager->ViewPitchMin = -30.f;
}
