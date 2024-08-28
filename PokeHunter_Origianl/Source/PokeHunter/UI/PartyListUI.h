// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PokeHunter/Base/BaseInstance.h"
#include "PokeHunter/Network/PartyServerManager.h"

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PartyListUI.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class POKEHUNTER_API UPartyListUI : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:
	UBaseInstance* gameinstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Party info")
		TArray<FName> Name_PartyList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Party info")
		TArray<int32> Count_PartyList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Party info")
		TArray<int32> inStage_PartyList;

	UFUNCTION(BlueprintCallable, Category = "Party")
		bool GetPartyList();
};
