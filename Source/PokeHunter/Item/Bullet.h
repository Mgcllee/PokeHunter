// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Bullet.generated.h"

/**
 * 
 */
UCLASS()
class POKEHUNTER_API ABullet : public AItem
{
	GENERATED_BODY()
	
public:
	ABullet();

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Battle")
	float Damage{5.f};

	float TimeLimit{5.f};

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UProjectileMovementComponent* ProjectileMovement;

	virtual void BeginPlay() override;

	UFUNCTION()
	//void OnHit(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);
	
	virtual void UseItem(APawn* ItemOwner, FVector InitialPos, FVector EndPos);

	UFUNCTION(NetMulticast, Reliable)
	void MultiLaunchBullet(FVector InitialPos, FVector EndPos);

	UFUNCTION(BlueprintNativeEvent)
	void OnHitNotEnemy(const FVector& HitVec);
	

	UFUNCTION(Server, Reliable)
	void ServerApplyDamage(AActor* DamagedActor, int DamageAmount, FVector Direction, const FHitResult& HitInfo, AController* EventInstigator, AActor* DamageCauser, TSubclassOf<UDamageType> DamageTypeClass);
	UFUNCTION(NetMulticast, Reliable)
	void MultiApplyDamage(AActor* DamagedActor, int DamageAmount, FVector Direction, const FHitResult& HitInfo, AController* EventInstigator, AActor* DamageCauser, TSubclassOf<UDamageType> DamageTypeClass);
};
