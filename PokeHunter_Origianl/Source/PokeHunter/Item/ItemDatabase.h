// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemData.h"
#include "Engine/DataAsset.h"
#include "ItemDatabase.generated.h"

/**
 * 
 */
UCLASS()
class POKEHUNTER_API UItemDatabase : public UDataAsset
{
	GENERATED_BODY()

protected:
	

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemData")
	TArray<TSubclassOf<class UItemData>> DataClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemData")
	TArray<FItemInfo> InfoArray;
};
