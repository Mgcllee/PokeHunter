// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HunterMainUI.generated.h"

/**
 * 
 */
UCLASS()
class POKEHUNTER_API UHunterMainUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Party Member info")
		TArray<int32> HunterHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Party Member info")
		TArray<int32> PetHP;

};
