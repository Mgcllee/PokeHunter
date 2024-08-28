// Fill out your copyright notice in the Description page of Project Settings.


#include "PartnerProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APartnerProjectile::APartnerProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.

	
	PrimaryActorTick.bCanEverTick = true;


	ProjectileMovement->InitialSpeed = 300.f;
	ProjectileMovement->MaxSpeed = 300.f;
	ProjectileMovement->bShouldBounce = false;

}

// Called when the game starts or when spawned
void APartnerProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APartnerProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APartnerProjectile::FireInDirection(const FVector& DirectionVec, const FVector& InitialPos, const FVector& EndPos)
{

	UGameplayStatics::SuggestProjectileVelocity(this, Velocity, InitialPos, EndPos,
		ProjectileMovement->InitialSpeed, false, 0.f, GetWorld()->GetGravityZ(), ESuggestProjVelocityTraceOption::DoNotTrace);
	//UGameplayStatics::SuggestProjectileVelocity_CustomArc(this, Velocity, InitialPos, EndPos, GetWorld()->GetGravityZ(), 1.f);
	ProjectileMovement->Velocity = Velocity;
	ProjectileMovement->SetVelocityInLocalSpace(Velocity);
	SetLifeSpan(TimeLimit);

	//경로 디버그 용
	/*FPredictProjectilePathParams predictParams(20.0f, InitialPos, Velocity, 15.0f);
	predictParams.DrawDebugTime = 15.0f;
	predictParams.DrawDebugType = EDrawDebugTrace::Type::ForDuration;
	predictParams.OverrideGravityZ = GetWorld()->GetGravity3Z();
	FPredictProjectilePathResult result;
	UGameplayStatics::PredictProjectilePath(this, predictParams, result);*/
	ProjectileMovement->UpdateComponentVelocity();
	StaticMesh->AddImpulse(Velocity, FName(""), true);
}

