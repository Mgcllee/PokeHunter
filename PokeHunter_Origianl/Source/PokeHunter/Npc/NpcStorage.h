// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PokeHunter/Base/BaseInstance.h"
#include "Npc.h"
#include "NpcStorage.generated.h"

UCLASS()
class POKEHUNTER_API ANpcStorage : public ANpc
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANpcStorage();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item")
	class UInventoryComponent* Storage;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void Interact_Implementation(AHunter* Hunter) override;
	
	UFUNCTION(BlueprintCallable)
		bool GetNpcStorageInfo(UBaseInstance* gameinstance);
};

