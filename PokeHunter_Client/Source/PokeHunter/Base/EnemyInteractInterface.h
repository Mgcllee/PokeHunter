// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EnemyInteractInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UEnemyInteractInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class POKEHUNTER_API IEnemyInteractInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void InteractEarthquake();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void InteractAttack(FVector HitDirection, float DamageAmount);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void InteractGrabAttack();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void InteractChargeAttack(float Distance);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void InteractWideAttack(float DamageAmount);
};
