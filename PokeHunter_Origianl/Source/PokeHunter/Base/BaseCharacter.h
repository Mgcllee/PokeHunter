// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

UCLASS()
class POKEHUNTER_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter();


	//Battle
	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadWrite, Category = "Status")
	float HP{ 100.f };
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Battle")
	bool bInvincible{ false };
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Battle")
	float InvincibleTime{ 1.f };
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle")
	float StartInvincibleTime;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Battle")
	float HealPerSecondAmount;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Battle")
	bool bDamaged;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Battle")
	bool bNoCollision;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Battle")
	float NoCollisionTime{ 0.5f };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	float StartNoCollisionTime;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//Replicated
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
};
