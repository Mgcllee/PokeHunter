// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "PokeHunter/Hunter/Hunter.h"

ABullet::ABullet()
{

	//Collision
	StaticMesh->SetCollisionProfileName("Bullet");

	//Movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 1000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	StaticMesh->OnComponentHit.AddDynamic(this, &ABullet::OnHit);
	//StaticMesh->OnComponentBeginOverlap.AddDynamic(this, &ABullet::OnHit);

};

void ABullet::BeginPlay()
{
	Super::BeginPlay();
}

void ABullet::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
//void ABullet::OnHit(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UGameplayStatics::ApplyPointDamage(OtherActor, Damage,GetActorForwardVector(),Hit,NULL,this,UDamageType::StaticClass());
	UE_LOG(LogTemp, Warning, TEXT("%d, %d,%d"), Hit.Location.X, Hit.Location.Y,Hit.Location.Z);
}

void ABullet::UseItem(APawn* ItemOwner, FVector InitialPos, FVector EndPos)
{
	FVector Velocity = FVector::ZeroVector;

	UGameplayStatics::SuggestProjectileVelocity(this, Velocity, InitialPos, EndPos, 
	ProjectileMovement->InitialSpeed, false, 0.f, GetWorld()->GetGravityZ(),ESuggestProjVelocityTraceOption::DoNotTrace);
	//UGameplayStatics::SuggestProjectileVelocity_CustomArc(this, Velocity, InitialPos, EndPos, GetWorld()->GetGravityZ(), 1.f);
	ThisOwner = ItemOwner;
	ProjectileMovement->Velocity = Velocity;
	ProjectileMovement->SetVelocityInLocalSpace(Velocity);
	SetLifeSpan(TimeLimit);

	//��� ����� ��
	/*FPredictProjectilePathParams predictParams(20.0f, InitialPos, Velocity, 15.0f);   
	predictParams.DrawDebugTime = 15.0f;    
	predictParams.DrawDebugType = EDrawDebugTrace::Type::ForDuration; 
	predictParams.OverrideGravityZ = GetWorld()->GetGravity3Z();
	FPredictProjectilePathResult result;
	UGameplayStatics::PredictProjectilePath(this, predictParams, result);*/
	ProjectileMovement->UpdateComponentVelocity();
	StaticMesh->AddImpulse(Velocity, FName(""),true);
}