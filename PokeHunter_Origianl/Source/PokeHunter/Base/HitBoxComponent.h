// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "HitBoxComponent.generated.h"

/**
 * 
 */
UCLASS()
class POKEHUNTER_API UHitBoxComponent : public UBoxComponent
{
	GENERATED_BODY()
	
public:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HitBox")
	class UHitBoxComponent* ChildHitbox;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Replicated, Category = "Part")
	float PartHP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Part")
	float Damage;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = "Part")
	bool bDestroyed;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PartBurning")
	float StartBurningTime;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PartBurning")
	float SaveBurningTime;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PartBurning")
	float BurningTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = "PartBurning")
	bool bBurning;

	

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const;

	UHitBoxComponent();
	bool TakeDamage(float DamageAmount);
	bool CheckBurning(float DeltaTime);

	void SetChild(class UHitBoxComponent* ChildBox);


	//Replication
	UFUNCTION(Server, Reliable)
	void ServerDestroyPart();
	UFUNCTION(NetMulticast, Reliable)
	void MultiDestroyPart();
	void DestroyPart();
	void DestroyPart(class AGolemBoss* Golem);

};
