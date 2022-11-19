// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ItemData.generated.h"

USTRUCT(BlueprintType)
struct FItemInfo
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
		FName ItemID;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
		FText Name;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
		TSubclassOf<class AItem> ItemClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
		TArray<FName> OfferingItemID;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
		UTexture2D* ItemIcon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
		int ItemAmount;
};


/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, DefaultToInstanced, EditInlineNew)
class POKEHUNTER_API UItemData : public UObject
{
	GENERATED_BODY()

public:
	UItemData();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
	FItemInfo ItemInfo;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 ItemIndex;

	UFUNCTION()
	FName getID() { return ItemInfo.ItemID; }

	void SetItemData(const AItem* Item, int32 Index);

};

