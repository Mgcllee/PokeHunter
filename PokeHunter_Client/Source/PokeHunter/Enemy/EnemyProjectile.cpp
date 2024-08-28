// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyProjectile.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AEnemyProjectile::AEnemyProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Collision
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));

	SetRootComponent(StaticMesh);
	StaticMesh->SetCollisionProfileName("EnemyProjectile");

	//Movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 500.f;
	ProjectileMovement->MaxSpeed = 500.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->ProjectileGravityScale = 1.f;

	if (HasAuthority())
	{
		StaticMesh->OnComponentHit.AddDynamic(this, &AEnemyProjectile::OnHit);
	}

	bReplicates = true;
}

// Called when the game starts or when spawned
void AEnemyProjectile::BeginPlay()
{
	Super::BeginPlay();

	bool bResult = FMath::RandBool();
	if (!bResult) Pitch = -1;
	bResult = FMath::RandBool();
	if (!bResult) Roll = -1;

}

// Called every frame
void AEnemyProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bRotating) return;
	FVector RotVec = FVector(Roll * RotateSpeed, Pitch * RotateSpeed, 0.f);
	StaticMesh->AddLocalRotation(RotVec.Rotation(), false);
}

void AEnemyProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	//if(OtherActor->CanBeDamaged())
	ServerApplyDamage(OtherActor, Damage, GetActorForwardVector(), Hit, NULL, this, UDamageType::StaticClass());

	ServerDestroy();
}

void AEnemyProjectile::ServerDestroy_Implementation()
{
	MultiDestroy();
}

void AEnemyProjectile::MultiDestroy_Implementation()
{
	Destroy();
}

void AEnemyProjectile::FirstUse(const FVector DirectionVec, const FVector& InitialPos, const FVector& EndPos)
{
	FireInDirection(DirectionVec, InitialPos, EndPos);
}

void AEnemyProjectile::FireInDirection(FVector DirectionVec, const FVector& InitialPos, const FVector& EndPos)
{
	FVector Velocity = FVector::ZeroVector;

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

void AEnemyProjectile::FireInDirection(FVector Direction)
{
	FVector Velocity = Direction * 3000.f;
	//ProjectileMovement->Velocity = Direction * 2000;
	//ProjectileMovement->UpdateComponentVelocity();
	SetLifeSpan(TimeLimit);
	StaticMesh->SetSimulatePhysics(true);
	StaticMesh->AddImpulse(Velocity, FName(""), true);
	StaticMesh->SetWorldRotation(Direction.Rotation());
	SetActorTickEnabled(true);
}

void AEnemyProjectile::InteractChargeAttack_Implementation(float DamageAmount)
{
}

void AEnemyProjectile::ServerApplyDamage_Implementation(AActor* DamagedActor, int DamageAmount, FVector Direction, const FHitResult& HitInfo, AController* EventInstigator, AActor* DamageCauser, TSubclassOf<UDamageType> DamageTypeClass)
{
	MultiApplyDamage(DamagedActor, DamageAmount, Direction, HitInfo, EventInstigator, DamageCauser, DamageTypeClass);
}

void AEnemyProjectile::MultiApplyDamage_Implementation(AActor* DamagedActor, int DamageAmount, FVector Direction, const FHitResult& HitInfo, AController* EventInstigator, AActor* DamageCauser, TSubclassOf<UDamageType> DamageTypeClass)
{
	UGameplayStatics::ApplyPointDamage(DamagedActor, DamageAmount, Direction, HitInfo, NULL, DamageCauser, DamageTypeClass);
}


