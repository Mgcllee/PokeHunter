// Fill out your copyright notice in the Description page of Project Settings.


#include "WolfPartner.h"
#include "PartnerProjectile.h"
#include "PartnerAnimInstance.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "PokeHunter/Enemy/Enemy.h"
#include "Kismet/GameplayStatics.h"

AWolfPartner::AWolfPartner()
{
	PrimaryActorTick.bCanEverTick = true;

	//����â Ŭ���� ����
	static ConstructorHelpers::FClassFinder<APartnerProjectile>TempIceShardClass(TEXT("/Game/Partner/Blueprint/PartnerProjectile/BP_IceShard"));
	if (TempIceShardClass.Succeeded())
	{
		IceShardClass = TempIceShardClass.Class;
	}


	//��ų ���� ������Ʈ
	StormCollision = CreateDefaultSubobject<UStaticMeshComponent>(FName("StormCollision"));
	StormCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StormCollision->SetupAttachment(GetMesh());
	BreathCollision = CreateDefaultSubobject<UStaticMeshComponent>(FName("BreathCollision"));
	BreathCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BreathCollision->SetupAttachment(GetMesh(), FName("HeadSocket"));
	BreathCollision->SetVisibility(false);

}


void AWolfPartner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (bBreathe)
	{
		int CurSecond = FMath::FloorToInt(BreatheLimitTime);
		BreatheLimitTime -= DeltaTime;
		if (CurSecond != FMath::FloorToInt(BreatheLimitTime))
		{
			UE_LOG(LogTemp, Warning, TEXT("Breathe is Running"));

			TArray<AActor*> OverlapActors;
			BreathCollision->GetOverlappingActors(OverlapActors, AEnemy::StaticClass());
			for (auto Enemy : OverlapActors)
			{
				ServerApplyDamage(Enemy, BreathDamage, GetController(), this);
			}
		}

		if (BreatheLimitTime <= 0.0f)
		{
			bBreathe = false;
			BreathCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			BreathCollision->SetVisibility(false);
		}
	}

	if (bOnStorm)
	{
		

		int CurSecond = FMath::FloorToInt(StormLimitTime);
		StormLimitTime -= DeltaTime;
		if (CurSecond != FMath::FloorToInt(StormLimitTime))
		{
			UE_LOG(LogTemp, Warning, TEXT("Storm is Running"));

			TArray<AActor*> OverlapActors;
			StormCollision->GetOverlappingActors(OverlapActors, AEnemy::StaticClass());
			for (auto Enemy : OverlapActors)
			{
				ServerApplyDamage(Enemy, StormDamage, GetController(), this);
			}
		}

		if (StormLimitTime <= 0.0f)
		{
			bOnStorm = false;
			StormCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			StormCollision->SetVisibility(false);
		}
	}

}

void AWolfPartner::BeginPlay()
{
	Super::BeginPlay();

	//StormCollision->OnComponentBeginOverlap.AddDynamic(this, &AWolfPartner::IntoStorm);
	//StormCollision->OnComponentEndOverlap.AddDynamic(this, &AWolfPartner::OutStorm);
}


void AWolfPartner::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(AWolfPartner, bBreathe);
	DOREPLIFETIME(AWolfPartner, BreatheLimitTime);
	DOREPLIFETIME(AWolfPartner, bOnStorm);
	DOREPLIFETIME(AWolfPartner, StormLimitTime);
}

void AWolfPartner::UseSpecialSkill(ESkillID SkillID)
{
	if (bUsingSkill) return;
	bOrdered = true;
	bUsingSkill = true;

	switch (SkillID)
	{
	case ESkillID::IceShard:
		TargetPos = Target->GetActorLocation();
		CurState = EPartnerState::IceShard;
		break;

	case ESkillID::IceStorm:
		CurState = EPartnerState::MakingStorm;
		break;

	case ESkillID::IceBreath:
		CurState = EPartnerState::IceBreath;
		break;
	default:
		break;
	}
}

void AWolfPartner::LaunchIceShard()
{
	if (PartnerAnim)
	{
		PartnerAnim->PlayCombatMontage(TEXT("Attack"));
	}
}

void AWolfPartner::IceBreathe()
{
	BreatheLimitTime = BreathTime;
	bBreathe = true;
	BreathCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BreathCollision->SetVisibility(true);
	ServerPlayMontage(FName("Breathe"));
}

void AWolfPartner::MakeIceShard()
{
	FVector InitialPos = GetMesh()->GetSocketLocation(FName("Head")) + GetActorForwardVector() * 300.f;
	FVector EndPos = Target->GetActorLocation();
	FVector DirectionVec = EndPos - GetActorLocation();
	DirectionVec.Normalize();

	ServerSpawnProjectile(this, IceShardClass, InitialPos, EndPos);
}

void AWolfPartner::MakeStorm()
{
	ServerPlayMontage(FName("IceStorm"));
	StormCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	StormCollision->SetVisibility(true);
	StormLimitTime = StormTime;
	bOnStorm = true;
	//StormCollision.set
}

