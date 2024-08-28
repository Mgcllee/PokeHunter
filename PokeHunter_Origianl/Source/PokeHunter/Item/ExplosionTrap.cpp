// Fill out your copyright notice in the Description page of Project Settings.


#include "ExplosionTrap.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PokeHunter/Enemy/Enemy.h"

AExplosionTrap::AExplosionTrap()
{
	CollisionBox->SetCollisionProfileName(FName("Trap"));
	
	Damage = 50.f;
}

void AExplosionTrap::BeginPlay()
{
	Super::BeginPlay();
}

void AExplosionTrap::Explode_Implementation()
{
}


void AExplosionTrap::ServerApplyDamage_Implementation(AActor* DamagedActor, int DamageAmount, class AController* ItemOwner, AActor* DamageCauser, TSubclassOf<UDamageType> DamageTypeClass)
{
	MultiApplyDamage(DamagedActor, DamageAmount, ItemOwner, DamageCauser, DamageTypeClass);
}

void AExplosionTrap::MultiApplyDamage_Implementation(AActor* DamagedActor, int DamageAmount, class AController* ItemOwner, AActor* DamageCauser, TSubclassOf<UDamageType> DamageTypeClass)
{
	UGameplayStatics::ApplyDamage(DamagedActor, DamageAmount,	ItemOwner, DamageCauser, DamageTypeClass);
}

float AExplosionTrap::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Explode();

	return DamageAmount;
}

void AExplosionTrap::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Explode();

}
