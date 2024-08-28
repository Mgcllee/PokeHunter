// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../PokeHunter.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "HunterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class POKEHUNTER_API UHunterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UHunterAnimInstance();
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	virtual void NativeInitializeAnimation() override;

	UFUNCTION(BlueprintCallable, Category = AnimationProperties)
	void UpdateAnimationProperties();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "Movement")
	float MovementSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "Movement")
	float Direction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "Combat")
	bool bZoom;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "Movement")
	bool bRunning;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "Movement")
	bool bFalling;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "Movement")
	bool bUpperOnly;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "Movement")
	class AHunter* Hunter;

	//Montage
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "Combat" )
	UAnimMontage* CombatMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "Interact")
	UAnimMontage* InteractMontage;

public:
	void PlayCombatMontage(FName Section);
	void PlayInteractMontage(FName Section);
};
