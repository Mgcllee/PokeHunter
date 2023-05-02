// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "PokeHunter/Base/ItemInteractInterface.h"
#include "PokeHunter/Base/HitBoxComponent.h"
#include "PokeHunter/Hunter/Hunter.h"
#include "PokeHunter/Partner/Partner.h"
#include "PokeHunter/Enemy/Enemy.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

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

	
	if (GetLocalRole() == ROLE_Authority)
	{
		StaticMesh->OnComponentHit.AddDynamic(this, &ABullet::OnHit);
	}
	//StaticMesh->OnComponentBeginOverlap.AddDynamic(this, &ABullet::OnHit);

	ItemType = EItemType::Bullet;

};

void ABullet::BeginPlay()
{
	Super::BeginPlay();
}

void ABullet::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{

	//UGameplayStatics::ApplyPointDamage(OtherActor, Damage, GetActorForwardVector(), Hit, NULL, this, UDamageType::StaticClass());
	if(OtherComponent->IsA<UHitBoxComponent>())
	UE_LOG(LogTemp, Warning, TEXT("HitBox Hit"), );
	ServerApplyDamage(OtherActor, Damage, GetActorForwardVector(), Hit, NULL, this, UDamageType::StaticClass());
	if (OtherActor->Implements<UItemInteractInterface>())
	{
		//������ ȿ���� �޴� ���Ϳ� �浹
		ApplyAbillity(OtherActor, OtherComponent);

		if (ParticleSystem)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleSystem, StaticMesh->GetComponentLocation());
		}
	}
	//������ ȿ���� ���� �ʴ� ���Ϳ� �浹
	else OnHitNotEnemy(Hit.Location);

	if (!bAttached)
	{
		ServerDestroy();
	}
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

void ABullet::MultiLaunchBullet_Implementation(APawn* BulletOwner, FVector InitialPos, FVector EndPos)
{
	FVector Velocity = FVector::ZeroVector;
	ThisOwner = BulletOwner;
	UGameplayStatics::SuggestProjectileVelocity(this, Velocity, InitialPos, EndPos,
		ProjectileMovement->InitialSpeed, false, 0.f, GetWorld()->GetGravityZ(), ESuggestProjVelocityTraceOption::DoNotTrace);
	//UGameplayStatics::SuggestProjectileVelocity_CustomArc(this, Velocity, InitialPos, EndPos, GetWorld()->GetGravityZ(), 1.f);
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
	StaticMesh->AddImpulse(Velocity, FName(""), true);
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
	FPointDamageEvent DamageEvent(DamageAmount, HitInfo, HitInfo.Normal, UDamageType::StaticClass());
	//DamagedActor->TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	UGameplayStatics::ApplyPointDamage(DamagedActor, DamageAmount, Direction, HitInfo, NULL, DamageCauser, DamageTypeClass);
}
