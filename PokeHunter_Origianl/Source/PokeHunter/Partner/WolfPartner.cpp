// Fill out your copyright notice in the Description page of Project Settings.


#include "WolfPartner.h"
#include "PartnerProjectile.h"
#include "PartnerAnimInstance.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "PokeHunter/Enemy/Enemy.h"
#include "PokeHunter/Hunter/Hunter.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"

#include "PokeHunter/Enemy/LongAttackEnemy.h"

AWolfPartner::AWolfPartner()
{
	PrimaryActorTick.bCanEverTick = true;

	//얼음창 클래스 설정
	static ConstructorHelpers::FClassFinder<APartnerProjectile>TempIceShardClass(TEXT("/Game/Partner/Blueprint/PartnerProjectile/BP_IceShard"));
	if (TempIceShardClass.Succeeded())
	{
		IceShardClass = TempIceShardClass.Class;
	}


	//스킬 범위 컴포넌트
	StormCollision = CreateDefaultSubobject<UStaticMeshComponent>(FName("StormCollision"));
	StormCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StormCollision->SetupAttachment(GetRootComponent());

	BreathCollision = CreateDefaultSubobject<UStaticMeshComponent>(FName("BreathCollision"));
	BreathCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BreathCollision->SetupAttachment(GetMesh(), FName("Head"));
	BreathCollision->SetVisibility(false);

	IceBreatheEffect = CreateDefaultSubobject<UNiagaraComponent>(FName("BreathEffect"));
	IceBreatheEffect->SetupAttachment(GetMesh(), FName("Head"));

	IceStormEffect = CreateDefaultSubobject<UNiagaraComponent>(FName("StormEffect"));
	IceStormEffect->SetupAttachment(GetRootComponent());
}


void AWolfPartner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//if (GetLocalRole() == ROLE_Authority)
	{

		if (bBreathe)
		{
			BreatheRuntime += DeltaTime;
			if (BreatheRuntime >= BreatheDamagePeriod)
			{
				TArray<AActor*> OverlapActors;
				BreathCollision->GetOverlappingActors(OverlapActors, AEnemy::StaticClass());
				for (auto Enemy : OverlapActors)
				{
					ServerApplyDamage(Enemy, BreathDamage, GetController(), this);
					ServerSpawnEffect(IceHitEffect, Enemy->GetActorLocation());
					if (Enemy->Implements<UItemInteractInterface>())
					{
						Execute_InteractIceSkill(Enemy);
					}
				}
				UE_LOG(LogTemp, Warning, TEXT("breathe: %d , cnt: %d"), bBreathe, BreatheDamageCnt);
				BreatheRuntime = 0.f;
				BreatheDamageCnt++;
			}

			if (BreatheDamageCnt >= BreathTime / BreatheDamagePeriod)
			{
				bBreathe = false;
				BreatheDamageCnt = 0;
				BreathCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				BreathCollision->SetVisibility(false);
				ServerStopMontage(this);
			}
		}

		if (bOnStorm)
		{
			StormRuntime += DeltaTime;
			if (StormRuntime >= StormDamagePeriod)
			{
				TArray<AActor*> OverlapActors;
				StormCollision->GetOverlappingActors(OverlapActors, AEnemy::StaticClass());
				for (auto Enemy : OverlapActors)
				{
					ServerApplyDamage(Enemy, StormDamage, GetController(), this);
					ServerSpawnEffect(IceHitEffect, Enemy->GetActorLocation());
					if (Enemy->Implements<UItemInteractInterface>())
					{
						Execute_InteractIceSkill(Enemy);
					}
				}
				StormRuntime = 0.f;
				StormDamageCnt++;
			}

			if (StormDamageCnt >= StormTime / StormDamagePeriod)
			{
				bOnStorm = false;
				StormDamageCnt = 0;
				StormCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				StormCollision->SetVisibility(false);
			}
		}
	}

}

void AWolfPartner::BeginPlay()
{
	Super::BeginPlay();

	//StormCollision->OnComponentBeginOverlap.AddDynamic(this, &AWolfPartner::IntoStorm);
	//StormCollision->OnComponentEndOverlap.AddDynamic(this, &AWolfPartner::OutStorm);

	IceBreatheEffect->SetFloatParameter(FName("SkillDuration"), BreathTime);
	IceBreatheEffect->SetVectorParameter(FName("BreathSize"), BreathCollision->GetComponentScale());
	IceStormEffect->SetFloatParameter(FName("SkillDuration"), StormTime);
	IceStormEffect->SetFloatParameter(FName("RadiusScale"), StormSize);
	
}


void AWolfPartner::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWolfPartner, bBreathe);
	DOREPLIFETIME(AWolfPartner, BreatheDamageCnt);
	DOREPLIFETIME(AWolfPartner, BreatheRuntime);
	DOREPLIFETIME(AWolfPartner, StormDamageCnt);
	DOREPLIFETIME(AWolfPartner, StormRuntime);
	//DOREPLIFETIME(AWolfPartner, BreatheLimitTime);
	DOREPLIFETIME(AWolfPartner, bOnStorm);
	//DOREPLIFETIME(AWolfPartner, StormLimitTime);
}

void AWolfPartner::UseSpecialSkill(ESkillID SkillID)
{
	if (bUsingSkill) return;
	bool bSuccess = false;

	switch (SkillID)
	{
	case ESkillID::Slash:
		if (Target)
		{
			TargetPos = Target->GetActorLocation();
			CurState = EPartnerState::SlashAttack;
			bUsingSkill = true;
			bOrdered = true;
			bSuccess = true;
		}
		break;
		 
	case ESkillID::IceShard:
		if (Target)
		{
			TargetPos = Target->GetActorLocation();
			CurState = EPartnerState::IceShard;
			bUsingSkill = true;
			bOrdered = true;
			bSuccess = true;
		}
		break;

	case ESkillID::IceStorm:
		CurState = EPartnerState::MakingStorm;
		bUsingSkill = true;
		bOrdered = true;
		bSuccess = true;
		break;

	case ESkillID::IceBreath:
		if (Target)
		{
			TargetPos = Target->GetActorLocation();
			CurState = EPartnerState::IceBreath;
			bUsingSkill = true;
			bOrdered = true;
			bSuccess = true;
		}
		break;
	default:
		break;
	}

	if (bSuccess)
	{
		Hunter->SuccessUseSkill(SkillID);
	}
}

void AWolfPartner::CancelOrder()
{
	Super::CancelOrder();
	IceBreatheEffect->DeactivateImmediate();
	BreatheDamageCnt = 0;
	ResetBreathe();
}

void AWolfPartner::LaunchIceShard()
{
	ServerPlayMontage(FName("IceShard"));
}

void AWolfPartner::IceBreathe()
{
	ServerPlayMontage(FName("Breathe"));
	
}

void AWolfPartner::ResetBreathe()
{
	//수정필
	IceBreatheEffect->DeactivateImmediate();
	BreatheDamageCnt = 0;
}

void AWolfPartner::ActivateBreathe()
{
	bBreathe = true;
	BreatheRuntime = 0;
	BreathCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	IceBreatheEffect->Activate(true);
}

void AWolfPartner::MakeIceShard()
{
	FVector InitialPos = GetMesh()->GetSocketLocation(FName("Head")) + GetActorForwardVector() * 300.f;
	FVector EndPos = Target->GetActorLocation();
	FVector DirectionVec = EndPos - InitialPos;
	FRotator Rotation = DirectionVec.Rotation();

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), IceHornEffect, InitialPos);

	//FQuat TargetRotation = FQuat::FindBetweenNormals(GetActorForwardVector(), DirectionVec);
	//FRotator Rotation = TargetRotation.Rotator();
	ServerSpawnProjectile(this, IceShardClass, InitialPos, EndPos, Rotation);
}

void AWolfPartner::MakeStorm()
{
	ServerPlayMontage(FName("IceStorm"));
}

void AWolfPartner::ActivateStorm()
{
	StormCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	bOnStorm = true;
	StormDamageCnt = 0;
	StormRuntime = 0;
	IceStormEffect->Activate(true);
}

