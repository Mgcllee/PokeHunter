// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PokeHunter/Base/ItemDropActor.h"
#include "Fairy.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EFairyType : uint8
{
	Fire,
	Ice,
	Poison
};

UCLASS()
class POKEHUNTER_API AFairy : public AItemDropActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		class UStaticMesh* CaughtStaticMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		class UParticleSystem* CaughtParticle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		EFairyType FairyType;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		FVector Direction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float RunningTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float Frequency;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float Amplitude;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float Speed;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		float TotalDistance;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		float Distance;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		float SaveZ;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float InteractRadius;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		bool bCaught{ false };

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		bool bRotating{ false };
	
public:
	AFairy();

	virtual void Tick(float DeltaTime) override;

	virtual void BeginPlay() override;

	virtual void Interact_Implementation(AHunter* Hunter) override;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,bool bFromSweep, const FHitResult& Hit);

	UFUNCTION(BlueprintCallable)
	void SetRandMove();
	void SinMove(float DeltaTime);


};
