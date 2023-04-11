// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "EnemyAnimInstance.h"
#include "EnemyProjectile.h"
#include "Net/UnrealNetwork.h"
#include "components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PokeHunter/Item/Item.h"
#include "PokeHunter/Base/ItemDropActor.h"
#include "PokeHunter/Hunter/Hunter.h"
#include "PokeHunter/Partner/Partner.h"

// Sets default values
AEnemy::AEnemy()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;



	GetCharacterMovement()->bOrientRotationToMovement = true;

	GetCapsuleComponent()->SetCapsuleRadius(90.f);
	//GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AEnemy::OnHit);


	TeamID = FGenericTeamId(1);
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	EnemyAnim = Cast<UEnemyAnimInstance>(GetMesh()->GetAnimInstance());
	EnemyAnim->OnMontageEnded.AddDynamic(this, &AEnemy::OnMontageEnded);

}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bBinding)
	{
		float ElapsedTime = GetWorld()->TimeSeconds - StartBindingTime;
		float TimeLeft = BindingTime - ElapsedTime;
		if (TimeLeft <= 0.0f)
		{
			bBinding = false;
			CurState = EEnemyState::Chase;
		}

	}

	//독 데미지 
	if (bPoisoned)
	{
		float ElapsedTime = GetWorld()->TimeSeconds - StartPoisonedTime;
		int CurSecond = FMath::FloorToInt(ElapsedTime);
		float TimeLeft = PoisonedTime - ElapsedTime;
		if (TimeLeft <= 0.0f)
		{
			bPoisoned = false;
			CurSecond = 0;
		}
		else
		{
			if (CurSecond == PoisonSaveTime)
			{
				//
			}
			else
			{
				PoisonSaveTime = CurSecond;
				FDamageEvent PoisonDamage;
				float DamageAmount = 1.f;
				TakeDamage(DamageAmount, PoisonDamage, NULL, NULL);

				//Damage UI Print Event
				OnDamage.Broadcast(DamageAmount, GetActorLocation());
			}
		}
	}
	//화상 데미지
	if (bBurning)
	{
		float ElapsedTime = GetWorld()->TimeSeconds - StartBurningTime;
		int CurSecond = FMath::FloorToInt(ElapsedTime);
		float TimeLeft = BurningTime - ElapsedTime;
		if (TimeLeft <= 0.0f)
		{
			bBurning = false;
			CurSecond = 0;
		}
		else
		{
			if (CurSecond == BurningSaveTime)
			{
				//
			}
			else
			{
				BurningSaveTime = CurSecond;
				FDamageEvent BurningDamage;
				float DamageAmount = 1.f;
				TakeDamage(DamageAmount, BurningDamage, NULL, NULL);

				//Damage UI Print Event
				OnDamage.Broadcast(DamageAmount, GetActorLocation());
			}
		}
	}

	if (bDied)
	{
		FVector CurScale = GetActorScale();
		SetActorScale3D(CurScale - DeltaTime * FVector(1.f, 1.f, 1.f));
		if (GetActorScale().Length() < 0.2f)
		{
			Die();
		}
	}
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const
{
	OutLocation = GetMesh()->GetSocketLocation("Head");
	OutRotation = GetMesh()->GetSocketRotation("Head");
}

FGenericTeamId AEnemy::GetGenericTeamId() const
{
	return TeamID;
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (HP <= 0) return 0;

	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	HP -= DamageAmount;
	//Item Hit
	AItem* HitItem = Cast<AItem>(DamageCauser);
	FVector HitLoc;

	
	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		const FPointDamageEvent& PointDamageEvent = static_cast<const FPointDamageEvent&>(DamageEvent);
		HitLoc = PointDamageEvent.HitInfo.Location;
		FVector DirectionVec = -1.f * PointDamageEvent.HitInfo.ImpactNormal;
		//DirectionVec.Z = 0.f;
		//DirectionVec.Normalize();
		//GetCharacterMovement()->Launch(DirectionVec * 1000.f);
		//LaunchCharacter(DirectionVec * 1000000.f, true,true);
	}
	else
	{
		HitLoc = GetActorLocation();
	}

	//피격 애니메이션 처리
	if (HitItem)
	{
		if (HP <= 0)
		{
			CurState = EEnemyState::Die;
			EnemyAnim->PlayCombatMontage(FName("Die"), true);
		}
		else
		{
			EnemyAnim->PlayCombatMontage(FName("Hit"), true);
			if (Target == NULL)
			{
				if (AHunter* Hunter = Cast<AHunter>(HitItem->ThisOwner))
				{
					Hunter->SetPartnerTarget(this);
				}
				Target = HitItem->ThisOwner;
				CurState = EEnemyState::Hit;
			}
		}
	}

	
	
	//Damage and UI
	OnDamage.Broadcast(DamageAmount, HitLoc);

	return DamageAmount;
}

bool AEnemy::IsJumping()
{
	return GetCharacterMovement()->IsFalling();
}

void AEnemy::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEnemy, CurState);
	DOREPLIFETIME(AEnemy, Target);

}

void AEnemy::ServerPlayMontage_Implementation(AEnemy* Enemy, FName Section)
{
	MultiPlayMontage(this, Section);
}


void AEnemy::MultiPlayMontage_Implementation(AEnemy* Enemy, FName Section)
{
	EnemyAnim->PlayCombatMontage(Section, true);
}

void AEnemy::ServerStartBinding_Implementation()
{
	MultiStartBinding_Implementation();
}

void AEnemy::MultiStartBinding_Implementation()
{
	bBinding = true;
	StartBindingTime = GetWorld()->TimeSeconds;
}

void AEnemy::StartBinding()
{
	ServerStartBinding();
}

void AEnemy::StartPoison()
{
	bPoisoned = true;
	StartPoisonedTime = GetWorld()->TimeSeconds;
}

void AEnemy::SeeNewTarget(AActor* Actor)
{
	TargetArray.AddUnique(Actor);
	
	
	if (Target == NULL)
	{
		SetTarget(Actor);
		CurState = EEnemyState::Chase;
		EnemyAnim->StopCombatMontage(0.2f);
	}
	
}

void AEnemy::HearSound(FVector SoundLoc, AActor* AgroActor)
{
	TargetPos = SoundLoc;
	AgroTarget = AgroActor;
	bWaitingAgro = true;
}

void AEnemy::Attack(int AttackPattern)
{
	if (Target != NULL)
	{
		CurState = EEnemyState::NormalAttack;
		ServerPlayMontage(this, FName("Attack"));
		//EnemyAnim->PlayCombatMontage(TEXT("Attack"));
	}
}

void AEnemy::LongAttack()
{
	if (EnemyAnim == NULL || Target == NULL) return;
	CurState = EEnemyState::LongAttack;
	FVector InitialPos = GetMesh()->GetSocketLocation(FName("LeftHand")) + GetActorForwardVector() * 300.f;
	FVector EndPos = Target->GetActorLocation();
	FVector DirectionVec = EndPos - GetActorLocation();
}

void AEnemy::Roar()
{
	if (EnemyAnim == NULL) return;
	ServerPlayMontage(this, FName("Roar"));
	//EnemyAnim->PlayCombatMontage(TEXT("Roar"));
}

void AEnemy::Patrol()
{
	if (EnemyAnim == NULL) return;
	ServerPlayMontage(this, FName("Patrol"));
	//EnemyAnim->PlayCombatMontage(TEXT("Patrol"));
}

void AEnemy::JumpAttack()
{
	CurState = EEnemyState::JumpAttack;
	ServerPlayMontage(this, FName("Attack_Jump"));
	
}


void AEnemy::LaunchToTarget()
{
	if (Target)
	{
		float Distance = GetDistanceTo(Target);
		FVector LookVec = Target->GetActorLocation() - GetActorLocation();
		LookVec.Normalize();
		LookVec.Z = 0.3f;
		FVector Velocity = LookVec * Distance;
		GetCharacterMovement()->Launch(Velocity);
	}
}

void AEnemy::Die()
{
	SpawnItem();
	Destroy();
}

void AEnemy::SpawnItem()
{
	FVector SpawnLoc = GetActorLocation();
	SpawnLoc.Z += 50.f;
	AItemDropActor* ItemBox = GetWorld()->SpawnActor<AItemDropActor>(DropItemBoxClass, SpawnLoc, GetActorRotation());
	ItemBox->CreateItemArray(DropItemID_Array);
}

void AEnemy::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!bInterrupted)
	{
		EnemyAnim->bPlaying = false;
		if (CurState == EEnemyState::Hit) CurState = EEnemyState::Roar;
		OnMontageEnd.Broadcast();
	}
}

void AEnemy::InteractIce_Implementation()
{
	GetCharacterMovement()->MaxWalkSpeed /= 2;
}

void AEnemy::InteractFire_Implementation(UPrimitiveComponent* HitComponent)
{
	bBurning = true;
	StartBurningTime = GetWorld()->TimeSeconds;
}

void AEnemy::InteractBindTrap_Implementation()
{
	bBinding = true;
	StartBindingTime = GetWorld()->TimeSeconds;
}