// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBomb.h"
#include "GameFramework/ProjectileMovementComponent.h"

AEnemyBomb::AEnemyBomb()
{
	SetActorTickEnabled(false);

	ProjectileMovement->InitialSpeed = 0.f;
	ProjectileMovement->MaxSpeed = 0.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->ProjectileGravityScale = 1.f;
	ProjectileMovement->Deactivate();

//	float Roll = FMath::RandRange(0.0f, 360.0f);
	//float Pitch = FMath::RandRange(0.0f, 360.0f);
	//float Yaw = FMath::RandRange(0.0f, 360.0f);
	//FRotator RandomRot = FRotator::MakeFromEuler(FVector(Roll, Pitch, Yaw));
	//StaticMesh->AddLocalRotation(RandomRot);
	StaticMesh->SetSimulatePhysics(false);

}

void AEnemyBomb::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bActive)
	{
		UpSpeed += -9.8f * GravityValue;
	}
	//AddActorWorldOffset(FVector(0, 0, DeltaTime * UpSpeed),true);
}

void AEnemyBomb::DropBomb()
{

}

void AEnemyBomb::FirstUse(const FVector DirectionVec, const FVector& InitialPos, const FVector& EndPos)
{
	DirVec = DirectionVec;
}

void AEnemyBomb::InteractChargeAttack_Implementation(float Distance)
{
	bActive = true;
	//ProjectileMovement->ProjectileGravityScale =1000.f;
	ProjectileMovement->Velocity = DirVec * Distance;
	ProjectileMovement->Activate();
	StaticMesh->SetSimulatePhysics(true);
}
