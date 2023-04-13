// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "PokeHunter/Base/ItemInteractInterface.h"
#include "PokeHunter/Hunter/Hunter.h"
#include "PokeHunter/Partner/Partner.h"
#include "PokeHunter/Enemy/Enemy.h"

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

	ItemType = EItemType::Bullet;

};

void ABullet::BeginPlay()
{
	Super::BeginPlay();
}

void ABullet::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	
	ServerApplyDamage(OtherActor, Damage, GetActorForwardVector(), Hit, NULL, this, UDamageType::StaticClass());
	if (OtherActor->Implements<UItemInteractInterface>())
	{
		//아이템 효과를 받는 액터와 충돌
		ApplyAbillity(OtherActor, OtherComponent);
		return;
	}
	//아이템 효과를 받지 않는 액터와 충돌
	OnHitNotEnemy(Hit.Location);
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

	//경로 디버그 용
	/*FPredictProjectilePathParams predictParams(20.0f, InitialPos, Velocity, 15.0f);   
	predictParams.DrawDebugTime = 15.0f;    
	predictParams.DrawDebugType = EDrawDebugTrace::Type::ForDuration; 
	predictParams.OverrideGravityZ = GetWorld()->GetGravity3Z();
	FPredictProjectilePathResult result;
	UGameplayStatics::PredictProjectilePath(this, predictParams, result);*/
	ProjectileMovement->UpdateComponentVelocity();
	StaticMesh->AddImpulse(Velocity, FName(""),true);
}

void ABullet::OnHitNotEnemy_Implementation(const FVector& HitVec)
{
	Destroy();
}


void ABullet::ServerApplyDamage_Implementation(AActor* DamagedActor, int DamageAmount, FVector Direction, const FHitResult& HitInfo, AController* EventInstigator, AActor* DamageCauser, TSubclassOf<UDamageType> DamageTypeClass)
{
	MultiApplyDamage(DamagedActor, DamageAmount, Direction, HitInfo, EventInstigator, DamageCauser, DamageTypeClass);
}

void ABullet::MultiApplyDamage_Implementation(AActor* DamagedActor, int DamageAmount, FVector Direction, const FHitResult& HitInfo, AController* EventInstigator, AActor* DamageCauser, TSubclassOf<UDamageType> DamageTypeClass)
{
	UGameplayStatics::ApplyPointDamage(DamagedActor, DamageAmount, Direction, HitInfo, NULL, DamageCauser, DamageTypeClass);
}
