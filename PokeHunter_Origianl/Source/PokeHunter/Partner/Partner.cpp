// Fill out your copyright notice in the Description page of Project Settings.


#include "Partner.h"
#include "PartnerController.h"
#include "PartnerAnimInstance.h"
#include "PartnerProjectile.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "PokeHunter/Hunter/Hunter.h"
#include "PokeHunter/Enemy/Enemy.h"

// Sets default values
APartner::APartner()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TeamID = FGenericTeamId(0);

	GetCharacterMovement()->bOrientRotationToMovement = true;

	baseinstace = Cast<UBaseInstance>(UGameplayStatics::GetGameInstance((GetWorld())));
}

// Called when the game starts or when spawned
void APartner::BeginPlay()
{
	Super::BeginPlay();

	PartnerAnim = Cast<UPartnerAnimInstance>(GetMesh()->GetAnimInstance());
	if (PartnerAnim)
	{
		PartnerAnim->OnMontageEnded.AddDynamic(this, &APartner::OnMontageEnded);
	}
}

// Called every frame
void APartner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bInvincible)
	{
		float ElapsedTime = GetWorld()->TimeSeconds - StartInvincibleTime;
		float TimeLeft = InvincibleTime - ElapsedTime;
		if (TimeLeft <= 0.0f)
		{
			bInvincible = false;
		}
	}

	if (bNoCollision)
	{
		float ElapsedTime = GetWorld()->TimeSeconds - StartNoCollisionTime;
		float TimeLeft = NoCollisionTime - ElapsedTime;
		if (TimeLeft <= 0.0f)
		{
			bNoCollision = false;
			SetActorEnableCollision(true);
		}
	}
}

// Called to bind functionality to input
void APartner::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void APartner::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APartner, Hunter);
	DOREPLIFETIME(APartner, CurState);
	DOREPLIFETIME(APartner, Target);
	DOREPLIFETIME(APartner, TargetPos);
	DOREPLIFETIME(APartner, bOrdered);
	DOREPLIFETIME(APartner, bUsingSkill);
	DOREPLIFETIME(APartner, bPosing);
	DOREPLIFETIME(APartner, bDied);
}

float APartner::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (bInvincible) return 0;

	//Bullet 그냥 리턴
	ABullet* Bullet = Cast<ABullet>(DamageCauser);
	if (Bullet) return 0.f;

	HP -= DamageAmount;
	Hunter->ServerPetHP(baseinstace->MyName, HP);

	if (GetHP() <= 0)
	{ //죽었을 때
		ServerPlayMontage(FName("Die"));
		bDied = true;
		SetGenericTeamId(1);
		HealPerSecondAmount = 0.f;
		//AEnemy* DamageEnemy = Cast<AEnemy>(DamageCauser);
		//if(DamageEnemy) DamageEnemy->LeaveTarget(this);
		return 0;
	}
	//입력 제한 필요
	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		FPointDamageEvent& PointDamageEvent = (FPointDamageEvent&)DamageEvent;
		ServerPlayMontage(FName("Hit"));
	}

	else
	{
		ServerPlayMontage( FName("Hit"));
	}


	if (bInvincible) return 0.0f;

	ServerStartPartnerInvincibility();

	return DamageAmount;
}

FGenericTeamId APartner::GetGenericTeamId() const
{
	return TeamID;
}

void APartner::ServerPlayMontage_Implementation(FName Section)
{
	MultiPlayMontage(Section);
}

void APartner::MultiPlayMontage_Implementation(FName Section)
{
	PartnerAnim->PlayCombatMontage(Section);
}

bool APartner::CheckFalling()
{
	bool bFalling = GetMovementComponent()->IsFalling();
	return bFalling;
}

void APartner::Attack()
{
	if (Target != NULL)
	{
		//Check Target in Range
		GetCharacterMovement()->MaxWalkSpeed = 2000;
		AttackPoint = Target->GetActorLocation();
		auto AIController = Cast<AAIController>(GetController());
		AIController->MoveToLocation(AttackPoint);


		//Play Montage
		ServerPlayMontage(FName("Attack"));
		//PartnerAnim->PlayCombatMontage(TEXT("Attack"));
	}

	
	
}

void APartner::SlashAttack()
{
	ServerPlayMontage(FName("SlashAttack"));

}

void APartner::SlashMove()
{
	if (Target)
	{
		FVector TargetLocation = Target->GetActorLocation();
		FVector Offset = GetActorLocation() - TargetLocation;
		FVector DirectionVec = TargetLocation - GetActorLocation();
		DirectionVec.Normalize();

		float SafeDistance = GetCapsuleComponent()->GetScaledCapsuleRadius() + Target->GetCapsuleComponent()->GetScaledCapsuleRadius();

		FVector StartLocation = GetActorLocation();
		FVector EndLocation = TargetLocation + DirectionVec * SafeDistance;
		FVector DesiredMovement = Offset + DirectionVec * SafeDistance;

		FHitResult HitResult;
		FRotator NewRotation = DirectionVec.Rotation();
		NewRotation.Pitch = 0;
		bool bHit = GetCapsuleComponent()->MoveComponent(DesiredMovement, NewRotation, true, &HitResult);

		if (bHit)
		{
			bHit = GetWorld()->LineTraceSingleByChannel(HitResult, EndLocation + FVector(0, 0, 100), EndLocation + FVector(0, 0, -1000), ECollisionChannel::ECC_Pawn);
			FVector NewLocation = HitResult.Location + FVector(0, 0, GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
			if (bHit)
			{
				SetActorLocation(NewLocation);
				ServerApplyDamage(Target, SlashDamage, GetController(), this);

				//이펙트 출력
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), SlashHitEffect, TargetLocation);
				FVector MoveEffectSpawnLoc = StartLocation + DirectionVec * MoveEffectOffsetX;
				MoveEffectSpawnLoc.Y += 50.f;
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), SlashMoveEffect, MoveEffectSpawnLoc, DirectionVec.Rotation());
			}
			else
			{
				SetActorLocation(StartLocation);
			}
			//실패 구현 필요
		}

		else
		{
			SetActorLocation(EndLocation);
			ServerPlayMontage(FName("SlashAttack"));

		}
	}
}

void APartner::Howling()
{
	//Play Montage
	ServerPlayMontage(FName("Howling"));
	//PartnerAnim->PlayCombatMontage(TEXT("Howling"));
}

void APartner::StopSkill()
{
	bOrdered = false;
	bUsingSkill = false;
	CurState = EPartnerState::MoveTarget;
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
}

void APartner::UseNormalSkill(ESkillID SkillID)
{
	if (bUsingSkill) return;

	bUsingSkill = true;
	bool bSuccess = false;

	switch (SkillID)
	{
	case ESkillID::Rush:

		CurState = EPartnerState::Rushing;
		bSuccess = true;
		break;

	case ESkillID::Howling:

		CurState = EPartnerState::Howling;
		bOrdered = true;
		bSuccess = true;
		break;

	case ESkillID:: Slash:
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

	default:
		break;
	}

	if (bSuccess)
	{
		Hunter->SuccessUseSkill(SkillID);
	}
}

void APartner::UseSpecialSkill(ESkillID SkillID)
{
	//스킬 사용중 접근 불가 추가 필요

	if (bUsingSkill) return;
	

	switch (SkillID)
	{
	case ESkillID::Rush:

		CurState = EPartnerState::Rushing;

		break;

	case ESkillID::Howling:

		CurState = EPartnerState::Howling;

		break;

	default:
		break;
	}
}

void APartner::SetTarget(ACharacter* setTarget)
{
	Target = setTarget;
}

void APartner::CancelOrder()
{
	bOrdered = false;
	PartnerAnim->StopAllMontages(0.5f);
	StopSkill();
}

void APartner::FollowHunter(AHunter* Master)
{
	Hunter = Master;
	bPosing = false;
	CurState = EPartnerState::MoveTarget;
}

void APartner::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	PartnerAnim->bPlaying = false;

	OnMontageEnd.Broadcast();

}

void APartner::InteractPotion_Implementation(float HealAmount)
{
	SetHP(GetHP() + HealAmount);
}

void APartner::InteractHealArea_Implementation()
{
	HealPerSecondAmount += 10.f;
}

void APartner::OutHealArea_Implementation()
{
	HealPerSecondAmount -= 10.f;
}

void APartner::ServerStartPartnerInvincibility_Implementation()
{
	MultiStartPartnerInvincibility();
}

void APartner::MultiStartPartnerInvincibility_Implementation()
{
	bInvincible = true;
	StartInvincibleTime = GetWorld()->TimeSeconds;
}

void APartner::MultiUseNormalSkill_Implementation(ESkillID SkillID)
{
	UseNormalSkill(SkillID);
}

void APartner::MultiUseSpecialSkill_Implementation(ESkillID SkillID)
{
	UseSpecialSkill(SkillID);
}

void APartner::MultiSetHunter_Implementation(class AHunter* OwnerHunter)
{
	SetOwner(OwnerHunter);
	FollowHunter(OwnerHunter);
}

void APartner::MultiSetPosition_Implementation(const FVector& LocVec)
{
	TargetPos = LocVec;
	bOrdered = true;
}

void APartner::ServerSetPosition_Implementation(const FVector& LocVec)
{
	MultiSetPosition(LocVec);
}

void APartner::InteractAttack_Implementation(FVector HitDirection, float DamageAmount)
{
	if (DamageAmount <= 0.f)
	{
		return;
	}

	if (bInvincible) return;

	if (HitDirection.Z < 0.f) HitDirection.Z *= -1;
	if (HitDirection.Z <= 0.3f) HitDirection.Z = 0.35f;

	//bDamaged = true;
	FVector TargetVec = FVector(HitDirection.X * -1, HitDirection.Y * -1, 0);
	FRotator TargetRot = TargetVec.Rotation();
	CurState = EPartnerState::Knockback;
	TargetRot.Pitch = 0;
	SetActorRelativeRotation(TargetRot);
	LaunchCharacter(HitDirection * 1000.f, false, false);
	StartNoCollisionTime = GetWorld()->GetTimeSeconds();
	bNoCollision = true;
	SetActorEnableCollision(false);
}

void APartner::InteractEarthquake_Implementation()
{
	if (GetCharacterMovement()->IsFalling()) return;

	if (bInvincible) return;

	LaunchCharacter(FVector(0, 0, 1000), false, false);
}

void APartner::InteractGrabAttack_Implementation()
{
	bNoCollision = true;
	StartNoCollisionTime = GetWorld()->GetTimeSeconds();
}

void APartner::InteractWideAttack_Implementation(float DamageAmount)
{
	if (DamageAmount <= 0.f)
	{
		return;
	}

	if (bInvincible) return;

	LaunchCharacter(FVector(0, 0, 1000), false, false);
	StartNoCollisionTime = GetWorld()->GetTimeSeconds();
	bNoCollision = true;
	SetActorEnableCollision(false);
}

void APartner::ServerSpawnProjectile_Implementation(APartner* OwnerPartner, TSubclassOf<class APartnerProjectile> SpawnProjectileClass, FVector StartLoc, FVector EndLoc, FRotator Rotation)
{
	//APartnerProjectile* Projectile = GetWorld()->SpawnActor<APartnerProjectile>(SpawnProjectileClass, StartLoc, Rotation);
	//Projectile->MultiLaunchBullet(OwnerPartner, StartLoc, EndLoc);
}

void APartner::ServerApplyDamage_Implementation(AActor* DamagedActor, float DamageAmount, AController* EventInstigator, AActor* DamageCauser)
{
	MultiApplyDamage(DamagedActor, DamageAmount, EventInstigator, DamageCauser);
}

void APartner::MultiApplyDamage_Implementation(AActor* DamagedActor, float DamageAmount, AController* EventInstigator, AActor* DamageCauser)
{
	UGameplayStatics::ApplyDamage(DamagedActor, DamageAmount, GetController(), DamageCauser, UDamageType::StaticClass());

}

void APartner::MultiCancelOrder_Implementation()
{
	CancelOrder();
}

void APartner::ServerSpawnEffect_Implementation(class UNiagaraSystem* Niagara, const FVector& SpawnLoc)
{
	MultiSpawnEffect(Niagara, SpawnLoc);
}

void APartner::MultiSpawnEffect_Implementation(class UNiagaraSystem* Niagara, const FVector& SpawnLoc)
{
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), Niagara, SpawnLoc);
}

void APartner::ServerStopMontage_Implementation(APartner* Partner)
{
	MultiStopMontage(Partner);
}

void APartner::MultiStopMontage_Implementation(APartner* Partner)
{
	PartnerAnim->StopAllMontages(0.2f);
}
