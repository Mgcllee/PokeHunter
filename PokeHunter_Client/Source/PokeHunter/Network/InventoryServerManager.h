// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Kismet/GameplayStatics.h"
#include "PokeHunter/Base/BaseInstance.h"
#include "PokeHunter/Base/DatabaseActor.h"
#include "PokeHunter/Hunter/Hunter.h"
#include "PokeHunter/Hunter/InventoryComponent.h"

#include "UObject/NoExportTypes.h"
#include "InventoryServerManager.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class POKEHUNTER_API UInventoryServerManager : public UObject
{
	GENERATED_BODY()
public:
	UInventoryServerManager();
	
	// UBaseInstance* gameinstance = nullptr;
	
	UFUNCTION(BlueprintCallable, Category = "inventory")
		bool GetInvenInfo(ACharacter* myPlayer, UBaseInstance* gameinstance);

	UFUNCTION(BlueprintCallable, Category = "inventory")
		bool SaveInventoryDB(UBaseInstance* gameinstance);
};
