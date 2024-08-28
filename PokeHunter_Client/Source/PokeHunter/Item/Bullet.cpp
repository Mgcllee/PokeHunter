// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
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
#include "Sound/SoundBase.h"

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
		//StaticMesh->OnComponentHit.AddDynamic(this, &ABullet::OnHit);
		StaticMesh->OnComponentBeginOverlap.AddDynamic(this, &ABullet::OnOverlapBegin);
	}

	ItemType = EItemType::Bullet;

};

void ABullet::BeginPlay()
{
	Super::BeginPlay();

	bool bResult = FMath::RandBool();
	if (!bResult) Pitch = -1;
	bResult = FMath::RandBool();
	if (!bResult) Roll = -1;
}

void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector RotVec = FVector(Roll * RotateSpeed, Pitch * RotateSpeed, 0.f);
	StaticMesh->AddLocalRotation(RotVec.Rotation(), false);
}

void ABullet::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitEffect, StaticMesh->GetComponentLocation(), GetActorRotation());

	if(SoundEffect) UGameplayStatics::PlaySoundAtLocation(GetWorld(), SoundEffect, StaticMesh->GetComponentLocation());
	if (!OtherActor) return;
	//UGameplayStatics::ApplyPointDamage(OtherActor, Damage, GetActorForwardVector(), Hit, NULL, this, UDamageType::StaticClass());
	if(OtherComponent->IsA<UHitBoxComponent>())
	UE_LOG(LogTemp, Warning, TEXT("HitBox Hit"), );
	ServerApplyDamage(OtherActor, Damage, GetActorForwardVector(), Hit, ThisOwner->GetController(), this, UDamageType::StaticClass());
	if (OtherActor->Implements<UItemInteractInterface>())
	{
		ApplyAbillity(OtherActor, OtherComponent);
	}
	else OnHitNotEnemy(Hit.Location);

	if (!bAttached)
	{
		ServerDestroy();
	}
}

void ABullet::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ServerSpawnEffect();
	if (!OtherActor || OtherActor == ThisOwner) return;
	//UGameplayStatics::ApplyPointDamage(OtherActor, Damage, GetActorForwardVector(), Hit, NULL, this, UDamageType::StaticClass());
	
	const FHitResult HitInfo = FHitResult(OtherActor, OtherComponent, StaticMesh->GetComponentLocation(), FVector(0, 0, 0));
	if (OtherActor->Implements<UItemInteractInterface>())
	{
		ApplyAbillity(OtherActor, OtherComponent);
	}
	else OnHitNotEnemy(HitInfo.Location);
	
	ServerApplyDamage(OtherActor, Damage, GetActorForwardVector(), HitInfo, ThisOwner->GetController(), this, UDamageType::StaticClass());
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
	UE_LOG(LogTemp, Warning, TEXT("==============================================================================================="));
	UE_LOG(LogTemp, Warning, TEXT("call MultiLaunchBullet_Implementation function"));
	FVector Velocity = FVector::ZeroVector;
	ThisOwner = BulletOwner;
	bool bSuccess =
	UGameplayStatics::SuggestProjectileVelocity(this, Velocity, InitialPos, EndPos,
		ProjectileMovement->InitialSpeed, false, 0.f, GetWorld()->GetGravityZ(), ESuggestProjVelocityTraceOption::DoNotTrace);

	UE_LOG(LogTemp, Warning, TEXT("launch success:  %d"), bSuccess);
	//UGameplayStatics::SuggestProjectileVelocity_CustomArc(this, Velocity, InitialPos, EndPos, GetWorld()->GetGravityZ(), 1.f);
	ProjectileMovement->Velocity = Velocity;
	ProjectileMovement->SetVelocityInLocalSpace(Velocity);

	UE_LOG(LogTemp, Warning, TEXT("Velocity.X : %.2f"), Velocity.X);
	UE_LOG(LogTemp, Warning, TEXT("Velocity.Y : %.2f"), Velocity.Y);
	UE_LOG(LogTemp, Warning, TEXT("Velocity.Z : %.2f"), Velocity.Z);

	UE_LOG(LogTemp, Warning, TEXT("ProjectileMovement->Velocity.X : %.2f"), ProjectileMovement->Velocity.X);
	UE_LOG(LogTemp, Warning, TEXT("ProjectileMovement->Velocity.Y : %.2f"), ProjectileMovement->Velocity.Y);
	UE_LOG(LogTemp, Warning, TEXT("ProjectileMovement->Velocity.Z : %.2f"), ProjectileMovement->Velocity.Z);

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
	UE_LOG(LogTemp, Warning, TEXT("==============================================================================================="));
}


void ABullet::ServerSpawnEmitter_Implementation(UParticleSystem* SpawnParticle, const FVector& SpawnLoc)
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SpawnParticle, SpawnLoc);
}

void ABullet::OnHitNotEnemy_Implementation(const FVector& HitVec)
{
	ServerDestroy();
}


void ABullet::ServerApplyDamage_Implementation(AActor* DamagedActor, int DamageAmount, FVector Direction, const FHitResult& HitInfo, AController* EventInstigator, AActor* DamageCauser, TSubclassOf<UDamageType> DamageTypeClass)
{
	MultiApplyDamage(DamagedActor, DamageAmount, Direction, HitInfo, EventInstigator, DamageCauser, DamageTypeClass);
}

void ABullet::MultiApplyDamage_Implementation(AActor* DamagedActor, int DamageAmount, FVector Direction, const FHitResult& HitInfo, AController* EventInstigator, AActor* DamageCauser, TSubclassOf<UDamageType> DamageTypeClass)
{
	FPointDamageEvent DamageEvent(DamageAmount, HitInfo, HitInfo.Normal, UDamageType::StaticClass());
	//DamagedActor->TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	UGameplayStatics::ApplyPointDamage(DamagedActor, DamageAmount, Direction, HitInfo, EventInstigator, DamageCauser, DamageTypeClass);
}

void ABullet::ServerSpawnEffect_Implementation()
{
	MultiSpawnEffect();
}

void ABullet::MultiSpawnEffect_Implementation()
{
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitEffect, StaticMesh->GetComponentLocation(), GetActorRotation());
}