// Fill out your copyright notice in the Description page of Project Settings.


#include "Trap.h"
#include "Components/BoxComponent.h"
#include "PokeHunter/Enemy/Enemy.h"
#include "Bullet.h"

void ATrap::BeginPlay()
{
	Super::BeginPlay();

	if (bHavingLife)
	{
		StartTime = GetWorld()->TimeSeconds;
		SetActorTickEnabled(true);
	}
	else
	{
		SetActorTickEnabled(false);
	}
}

ATrap::ATrap()
{
	ItemType = EItemType::Trap;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(FName("CollisionBox"));
	CollisionBox->SetupAttachment(GetRootComponent());
	
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ATrap::OnBeginOverlap);
}

void ATrap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckLifeTime(DeltaTime);

}

void ATrap::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	/*if (OtherActor->IsA<AEnemy>() || OtherActor->IsA<ABullet>())
	{
		this->Destroy();
	}*/
}

void ATrap::CheckLifeTime(float DeltaTime)
{
	float ElapsedTime = GetWorld()->TimeSeconds - StartTime;
	float TimeLeft = LifeTime - ElapsedTime;
	if (TimeLeft < 0.0f) Destroy();
}

