// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Trap.h"
#include "ExplosionTrap.generated.h"

/**
 * 
 */
UCLASS()
class POKEHUNTER_API AExplosionTrap : public ATrap
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Collision")
	class USphereComponent* ExplosionCollision;

public:
	AExplosionTrap();

	UFUNCTION()
	virtual void OnExplosionOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
