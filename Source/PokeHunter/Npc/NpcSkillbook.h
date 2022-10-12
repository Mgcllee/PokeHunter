// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Npc.h"
#include "NpcSkillbook.generated.h"

/**
 * 
 */
UCLASS()
class POKEHUNTER_API ANpcSkillbook : public ANpc
{
	GENERATED_BODY()

public:
	ANpcSkillbook();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void Interact_Implementation(AHunter* Hunter) override;
};
