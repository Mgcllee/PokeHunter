// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PokeHunter/Item/Bullet.h"
#include "PartnerProjectile.generated.h"

UCLASS()
class POKEHUNTER_API APartnerProjectile : public ABullet
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APartnerProjectile();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Velocity;



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void FireInDirection(const FVector& DirectionVec, const FVector& InitialPos, const FVector& EndPos);

};
