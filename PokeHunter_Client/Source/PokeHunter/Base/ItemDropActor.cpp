// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemDropActor.h"
#include "Net/UnrealNetwork.h"
#include "PokeHunter/Hunter/Hunter.h"
#include "PokeHunter/Hunter/InventoryComponent.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "Components/StaticMeshComponent.h"

AItemDropActor::AItemDropActor()
{
	PrimaryActorTick.bCanEverTick = true;

	//Mesh
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	SetRootComponent(StaticMesh);

	InteractionSphere->SetupAttachment(RootComponent);
	InteractionSphere->SetSphereRadius(80.f);

	RotatingMovement = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("Rotating Movement"));
	AddOwnedComponent(RotatingMovement);

}

void AItemDropActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bInteracting)
	{
		FVector CurPointVec = CalculatePoint(DeltaTime);
		SetActorLocation(CurPointVec);
		StaticMesh->SetWorldLocation(CurPointVec);

		if (RunningTime >= TotalTime)
		{
			if (DropItemArray.IsEmpty())
			{
				//임시 차단
				ServerDestroy();
				return;
			}

			DropCnt = FMath::RandRange(1, 5);
			for (int i = 0; i < DropCnt; ++i)
			{
				if (DropItemArray.IsEmpty()) break;
				int RandIndex = FMath::RandRange(0, DropItemArray.Num()-1);
				DropItemArray[RandIndex].cnt += 1;
			}

			for (int i = 0; i < DropItemArray.Num(); ++i)
			{
				bool bAddSuccess = Master->Inventory->AddItemData(DropItemArray[i]);
				if (!bAddSuccess)
				{
					return;
				}

			}
			//획득 처리 모션 추가 필요
			ServerDestroy();
			return;
		}
	}
}

void AItemDropActor::BeginPlay()
{
	Super::BeginPlay();

	//�����ڿ��� �����۵����� Ŭ���� �߰� �� �����ڿ��� ����
	int32 RaritySum = 0;
	for (auto& DataMap : DropItemMap)
	{
		RaritySum += DataMap.Key;
	}
	if (RaritySum != 0) BaseProbability = int32(100 / RaritySum);
}

void AItemDropActor::CreateItemArray(const TArray<FName>& ItemArray)
{
	for (int i = 0; i < ItemArray.Num(); ++i)
	{
		FItemCnter ItemCnter;
		ItemCnter.ItemID = ItemArray[i];
		DropItemArray.Add(ItemCnter);
	}
	
}

FVector AItemDropActor::CalculatePoint(float DeltaTimes)
{
	FVector EndPointVec = Master->GetActorLocation();

	float RemainTime = (TotalTime - RunningTime) / TotalTime;

	FVector CurPointVec = RemainTime * RemainTime * StartPointVec + 2 * RemainTime * RunningTime * TurningPointVec + RunningTime * RunningTime * EndPointVec;

	RunningTime += DeltaTimes;

	return CurPointVec;
}

void AItemDropActor::Interact_Implementation(AHunter* Hunter)
{
	Master = Hunter;
	StartPointVec = GetActorLocation();
	bInteracting = true;
	StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TurningPointVec = Master->GetActorLocation() + FVector(0, 300, 200);
}

void AItemDropActor::MultiInteract_Implementation(AHunter* Hunter)
{
	Master = Hunter;
	StartPointVec = GetActorLocation();
	bInteracting = true;
	SetActorTickEnabled(true);
	StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TurningPointVec = Master->GetActorLocation() + FVector(0, 300, 200);
}

void AItemDropActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AItemDropActor, Master);
	DOREPLIFETIME(AItemDropActor, bInteracting);
	DOREPLIFETIME(AItemDropActor, DropItemArray);
	DOREPLIFETIME(AItemDropActor, StartPointVec);
	DOREPLIFETIME(AItemDropActor, TurningPointVec);
}

void AItemDropActor::ServerDestroy_Implementation()
{
	MultiDestroy();
}

void AItemDropActor::MultiDestroy_Implementation()
{
	Destroy();
}
