// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ItemData.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, DefaultToInstanced, EditInlineNew)
class POKEHUNTER_API UItemData : public UObject
{
	GENERATED_BODY()

public:
	UItemData();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 ItemIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* ItemIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AItem> ItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCombine;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<UItemData*, int> MaterialItemMap;

	void SetItemData(const AItem* Item, int32 Index);

};

