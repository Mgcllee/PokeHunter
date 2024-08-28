// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/GameplayStatics.h"
#include "PokeHunter/Base/BaseInstance.h"

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PartyServerManager.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class POKEHUNTER_API UPartyServerManager : public UObject
{
	GENERATED_BODY()
	
public:
	UPartyServerManager();

	UBaseInstance* gameinstance = nullptr;

	UFUNCTION(BlueprintCallable)
		bool GetPartyList();
};
