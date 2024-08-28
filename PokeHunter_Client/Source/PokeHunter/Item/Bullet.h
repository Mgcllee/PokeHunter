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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	class UNiagaraSystem* HitEffect;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sound")
	class USoundBase* SoundEffect;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Battle")
	float Damage{5.f};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	float TimeLimit{5.f};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	bool bAttached{ false };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		int Pitch{ 1 };
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		int Roll{ 1 };
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float RotateSpeed{ 1000 };


public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UProjectileMovementComponent* ProjectileMovement;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	//void OnHit(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void UseItem(APawn* ItemOwner, FVector InitialPos, FVector EndPos);

	UFUNCTION(NetMulticast, Reliable)
	void MultiLaunchBullet(APawn* BulletOwner, FVector InitialPos, FVector EndPos);
	virtual void MultiLaunchBullet_Implementation(APawn* BulletOwner, FVector InitialPos, FVector EndPos);

	UFUNCTION(BlueprintNativeEvent)
	void OnHitNotEnemy(const FVector& HitVec);
	
	//Replicate

	UFUNCTION(Server, Reliable)
	void ServerApplyDamage(AActor* DamagedActor, int DamageAmount, FVector Direction, const FHitResult& HitInfo, AController* EventInstigator, AActor* DamageCauser, TSubclassOf<UDamageType> DamageTypeClass);
	UFUNCTION(NetMulticast, Reliable)
	void MultiApplyDamage(AActor* DamagedActor, int DamageAmount, FVector Direction, const FHitResult& HitInfo, AController* EventInstigator, AActor* DamageCauser, TSubclassOf<UDamageType> DamageTypeClass);

	UFUNCTION(Server, Reliable)
	void ServerSpawnEmitter(UParticleSystem* SpawnParticle, const FVector& SpawnLoc);

	UFUNCTION(Server, Reliable)
	void ServerSpawnEffect();
	UFUNCTION(NetMulticast, Reliable)
	void MultiSpawnEffect();
	
};
