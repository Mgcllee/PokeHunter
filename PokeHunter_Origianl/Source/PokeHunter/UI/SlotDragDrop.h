// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "SlotDragDrop.generated.h"

/**
 * 
 */
UCLASS()
class POKEHUNTER_API USlotDragDrop : public UDragDropOperation
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int SlotNum;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UItemData* ItemData;
	
};
