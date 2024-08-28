// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "LongAttackEnemy.generated.h"

/**
 * 
 */
UCLASS()
class POKEHUNTER_API ALongAttackEnemy : public AEnemy
{
	GENERATED_BODY()
	

public:
	ALongAttackEnemy();

	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable)
	void LaunchProjectile();
};
