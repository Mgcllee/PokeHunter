// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PokeHunter/PokeHunter.h"
#include "GameFramework/PlayerController.h"
#include "HunterController.generated.h"

/**
 * 
 */
UCLASS()
class POKEHUNTER_API AHunterController : public APlayerController
{
	GENERATED_BODY()
	
protected:

public:
	FName MyName;

public:
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
};
