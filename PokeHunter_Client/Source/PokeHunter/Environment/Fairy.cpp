// Fill out your copyright notice in the Description page of Project Settings.


#include "Fairy.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystem.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "PokeHunter/Item/Item.h"

AFairy::AFairy()
{
	PrimaryActorTick.bCanEverTick = true;

	InteractionSphere->SetSphereRadius(10.f);
	InteractionSphere->SetCollisionProfileName(FName("OverlapAllDynamic"));
	InteractionSphere->OnComponentBeginOverlap.AddDynamic(this, &AFairy::OnHit);
}

void AFairy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//선형 이동
	if (bRotating)
	{
		SetActorRotation(FMath::RInterpTo(GetActorRotation(), Direction.Rotation(), DeltaTime, 1.f));
		float BetweenAngle = FMath::Acos(FVector::DotProduct(Direction, GetActorForwardVector()) / (Direction.Size() * GetActorForwardVector().Size())) * (180.0f / PI);
		if (BetweenAngle < 5.f)
		{
			SetActorRotation(Direction.Rotation());
			bRotating = false;
		}
	}
	else
	{
		SinMove(DeltaTime);
	}

}

void AFairy::BeginPlay()
{
	Super::BeginPlay();

	SetRandMove();

	
}

void AFairy::Interact_Implementation(AHunter* Hunter)
{
	if (bCaught)
	{
		AItemDropActor::Interact_Implementation(Hunter);
	}
}

void AFairy::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	AItem* HitItem = Cast<AItem>(OtherActor);
	if (HitItem)
	{
		//다른 아이템과 충돌 시
		if (HitItem->ItemType != EItemType::Bullet) //정확하게 수정
		{
			//죽음 처리
			InteractionSphere->SetSimulatePhysics(true);
			InteractionSphere->SetCollisionProfileName(FName("SmallItem"));


			
		}

		//포획 아이템과 충돌 시
		else
		{
			SetActorTickEnabled(false);
			StaticMesh->SetStaticMesh(CaughtStaticMesh);
			InteractionSphere->SetSimulatePhysics(true);
			InteractionSphere->SetCollisionProfileName(FName("SmallItem"));

			if (CaughtParticle)
			{
				//UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), CaughtParticle, GetActorTransform());
			}
			bCaught = true;
		}
	}
}

void AFairy::SetRandMove()
{
	//초기 방향벡터 설정
	Direction = FMath::VRand();
	Direction.Z = 0;
	Direction.Normalize();

	Amplitude = FMath::RandRange(100.0f, 150.0f);

	Distance = FMath::RandRange(800.f,1000.f);

	TotalDistance = 0;

	RunningTime = 0;

	bRotating = true;

	SaveZ = GetActorLocation().Z;
	//SetActorRotation(Direction.Rotation());
}

void AFairy::SinMove(float DeltaTime)
{
	FVector PrevLocation = GetActorLocation();
	FVector PrevDirection = Direction;

	RunningTime += DeltaTime;

	FVector NewLocation;
	NewLocation.X = PrevLocation.X + Direction.X * Speed * DeltaTime;
	NewLocation.Y = PrevLocation.Y + Direction.Y * Speed * DeltaTime;

	

	float MoveDelta = FMath::Sin(Speed * RunningTime * Frequency) * Amplitude;
	NewLocation.Z = SaveZ + MoveDelta;

	SetActorLocation(NewLocation);

	//SetActorRotation(Direction.Rotation());


	
	float CurDistance = FVector::Dist(NewLocation,PrevLocation);
	TotalDistance += CurDistance;

	if (TotalDistance > Distance)
	{
		SetRandMove();
	}
	
}
