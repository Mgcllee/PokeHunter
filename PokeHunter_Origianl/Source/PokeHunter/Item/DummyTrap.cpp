// Fill out your copyright notice in the Description page of Project Settings.


#include "DummyTrap.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"

ADummyTrap::ADummyTrap()
{
	TeamID = FGenericTeamId(0);
}

void ADummyTrap::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}

float ADummyTrap::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	HP -= DamageAmount;
	if (HP <= 0.f)	
	{
		Explode();
	}

	return DamageAmount;
}

