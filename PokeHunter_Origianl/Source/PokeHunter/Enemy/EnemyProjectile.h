// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PokeHunter/Base/EnemyInteractInterface.h"
#include "EnemyProjectile.generated.h"

UCLASS()
class POKEHUNTER_API AEnemyProjectile : public AActor, public IEnemyInteractInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemyProjectile();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class APawn* ThisOwner;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mesh")
	class UStaticMeshComponent* StaticMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	float Damage{ 30.f };

	float TimeLimit{ 5.f };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		int Pitch{ 1 };
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		int Roll{ 1 };
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float RotateSpeed{ 500 };

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		bool bRotating{ true };

public:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION(Server, Reliable)
	void ServerApplyDamage(AActor* DamagedActor, int DamageAmount, FVector Direction, const FHitResult& HitInfo, AController* EventInstigator, AActor* DamageCauser, TSubclassOf<UDamageType> DamageTypeClass);
	UFUNCTION(NetMulticast, Reliable)
	void MultiApplyDamage(AActor* DamagedActor, int DamageAmount, FVector Direction, const FHitResult& HitInfo, AController* EventInstigator, AActor* DamageCauser, TSubclassOf<UDamageType> DamageTypeClass);

	//Replicate
	UFUNCTION(Server, Reliable)
	void ServerDestroy();
	UFUNCTION(NetMulticast, Reliable)
	void MultiDestroy();

	virtual void FirstUse(const FVector DirectionVec, const FVector& InitialPos, const FVector& EndPos);
	virtual void FireInDirection(FVector DirectionVec, const FVector& InitialPos, const FVector& EndPos);
	virtual void FireInDirection(FVector Direction);

	virtual void InteractChargeAttack_Implementation(float DamageAmount) override;
};
