// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Trap.generated.h"

/**
 * 
 */
UCLASS()
class POKEHUNTER_API ATrap : public AItem
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Collision")
	class UBoxComponent* CollisionBox;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Duration")
	float LifeTime{ 5.f };
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Duration")
	float StartTime;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Duration")
	bool bHavingLife{ false };

protected:
	virtual void BeginPlay() override;

public:
	ATrap();
	virtual void Tick(float DeltaTime) override;
	UFUNCTION()
	virtual void OnBeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	void CheckLifeTime(float DeltaTime);



};
