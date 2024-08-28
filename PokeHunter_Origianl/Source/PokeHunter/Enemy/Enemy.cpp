// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "EnemyAnimInstance.h"
#include "EnemyProjectile.h"
#include "EnemyController.h"
#include "Net/UnrealNetwork.h"
#include "components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PokeHunter/Item/Item.h"
#include "PokeHunter/Base/ItemDropActor.h"
#include "PokeHunter/Base/BaseCharacter.h"
#include "PokeHunter/Hunter/Hunter.h"
#include "PokeHunter/Partner/Partner.h"
#include <PokeHunter/Base/HunterState.h>

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

	SubSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SubSkeletalMesh"));
	SubSkeletalMesh->SetupAttachment(GetMesh());

	TeamID = FGenericTeamId(1);
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	UpdateOverlaps(true);

	EnemyAnim = Cast<UEnemyAnimInstance>(GetMesh()->GetAnimInstance());
	if (EnemyAnim) EnemyAnim->OnMontageEnded.AddDynamic(this, &AEnemy::OnMontageEnded);
	
	if (SubSkeletalMesh) EnemySubAnim = Cast<UEnemyAnimInstance>(SubSkeletalMesh->GetAnimInstance());
	BaseLocation = GetActorLocation();

	BaseWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	//ComeBackHome();
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float CurTime = GetWorld()->TimeSeconds;
	if (bBinding)
	{
		float ElapsedTime = CurTime - StartBindingTime;
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
		int CurSecond = FMath::FloorToInt(PoisonLimitTime);
		PoisonLimitTime -= DeltaTime;
		if (CurSecond != FMath::FloorToInt(PoisonLimitTime))
		{
			FDamageEvent PoisonedDamage;
			float DamageAmount = 1.f;
			UE_LOG(LogTemp, Warning, TEXT("PoisonedDamage"));
			if (HasAuthority())
			{
				UGameplayStatics::ApplyDamage(this, DamageAmount, nullptr, nullptr, UDamageType::StaticClass());
			}

			else
			{
				ServerApplyDamage(this, DamageAmount, nullptr, nullptr, UDamageType::StaticClass());
			}
		}
		if (PoisonLimitTime <= 0.0f)
		{
			bPoisoned = false;
		}
	}


	//화상 데미지
	if (bBurning)
	{
		int CurSecond = FMath::FloorToInt(BurningLimitTime);
		BurningLimitTime -= DeltaTime;
		if (CurSecond != FMath::FloorToInt(BurningLimitTime))
		{
			FDamageEvent BurningDamage;
			float DamageAmount = 1.f;
			UE_LOG(LogTemp, Warning, TEXT("BurningDamage"));
			if (HasAuthority())
			{
				UGameplayStatics::ApplyDamage(this, DamageAmount, nullptr, nullptr, UDamageType::StaticClass());
			}

			else
			{
				ServerApplyDamage(this, DamageAmount, nullptr, nullptr, UDamageType::StaticClass());
			}
		}
		if (BurningLimitTime <= 0.0f)
		{
			bBurning = false;
		}
	}

	//둔화
	if (bSlow)
	{
		int CurSecond = FMath::FloorToInt(SlowLimitTime);
		SlowLimitTime -= DeltaTime;
		if (SlowLimitTime <= 0.0f)
		{
			bSlow = false;
			GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
		}
	}

	//빙결
	if (bFrozen)
	{
		int CurSecond = FMath::FloorToInt(FrozenLimitTime);
		FrozenLimitTime -= DeltaTime;
		if (FrozenLimitTime <= 0.0f)
		{
			bFrozen = false;
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
	OutLocation = GetMesh()->GetSocketLocation("HeadSocket");
	OutRotation = GetMesh()->GetSocketRotation("HeadSocket");
}

FGenericTeamId AEnemy::GetGenericTeamId() const
{
	return TeamID;
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (HP <= 0 || CurState == EEnemyState::Die) return 0;
	if (bWeaken)
	{
		DamageAmount *= 2;
		bWeaken = false;
	}
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	HP -= DamageAmount;
	SavedDamage += DamageAmount;

	//Item Hit && HitLocation
	AItem* HitItem = Cast<AItem>(DamageCauser);

	if (nullptr != HitItem) {
		AHunter* hunterDamageStack = Cast<AHunter>(HitItem->ThisOwner);
		if (hunterDamageStack) {
			AHunterState* playerState = Cast<AHunterState>(hunterDamageStack->GetPlayerState());
			if (playerState) {
				playerState->damageInfo.hunterAmount += DamageAmount;
			}
		}

		APartner* HitPartner = Cast<APartner>(DamageCauser);
		if (nullptr != HitPartner) {
			AHunterState* playerState = Cast<AHunterState>(GetPlayerState());
			if (playerState) {
				playerState->damageInfo.petAmount += DamageAmount;
			}
		}
	}
	
	FVector HitLoc;
	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		const FPointDamageEvent& PointDamageEvent = static_cast<const FPointDamageEvent&>(DamageEvent);
		HitLoc = PointDamageEvent.HitInfo.Location;
		FVector DirectionVec = -1.f * PointDamageEvent.HitInfo.ImpactNormal;
	}
	else
	{
		HitLoc = GetActorLocation();
	}

	//Change EnemyState
	if (HP <= 0)
	{
		//사망 애니메이션
		CurState = EEnemyState::Die;
		bDied = true;
		ServerPlayMontage(this, FName("Die"));
		auto* EnemyController = Cast<AEnemyController>(GetController());
		if (EnemyController) EnemyController->StopAI();
	}


	else
	{ 
	//피격 애니메이션 처리
	if (HitItem)
	{
		CurState = EEnemyState::Hit;
		AHunter* Hunter = Cast<AHunter>(HitItem->ThisOwner);
		if (Hunter)
		{
			Hunter->SetPartnerTarget(this);
		}

		if (Target == NULL)
		{
			Target = Hunter;
		}
	}

		if (bGrogy)
		{
			//그로기 시 애니메이션 생략
		}
		else
		{
			ServerPlayMontage(this, FName("Hit"));
		}
	}

	//Damage and UI
	ServerShowDamage(DamageAmount, HitLoc);

	return DamageAmount;
}

bool AEnemy::IsJumping()
{
	return GetCharacterMovement()->IsFalling();
}

void AEnemy::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEnemy, HP);
	DOREPLIFETIME(AEnemy, CurState);
	DOREPLIFETIME(AEnemy, Target);
	DOREPLIFETIME(AEnemy, bDied);
	DOREPLIFETIME(AEnemy, bBurning);
	DOREPLIFETIME(AEnemy, BurningLimitTime);
	DOREPLIFETIME(AEnemy, bPoisoned);
	DOREPLIFETIME(AEnemy, PoisonLimitTime);
	DOREPLIFETIME(AEnemy, bSlow);
	DOREPLIFETIME(AEnemy, SlowLimitTime);
	DOREPLIFETIME(AEnemy, bFrozen);
	DOREPLIFETIME(AEnemy, FrozenLimitTime);
	DOREPLIFETIME(AEnemy, bWeaken);
}

void AEnemy::ServerPlayMontage_Implementation(AEnemy* Enemy, FName Section)
{
	MultiPlayMontage(this, Section);
}


void AEnemy::MultiPlayMontage_Implementation(AEnemy* Enemy, FName Section)
{
	EnemyAnim->PlayCombatMontage(Section, true);
	if (EnemySubAnim) EnemySubAnim->PlayCombatMontage(Section, true);
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

void AEnemy::ServerApplyPointDamage_Implementation(AActor* OtherActor, float DamageAmount, FVector HitDirection, AActor* DamageCauser, const FHitResult& SweepResult)
{
	MultiApplyPointDamage(OtherActor, DamageAmount, HitDirection, DamageCauser, SweepResult);
}

void AEnemy::MultiApplyPointDamage_Implementation(AActor* OtherActor, float DamageAmount, FVector HitDirection, AActor* DamageCauser, const FHitResult& SweepResult)
{
	UGameplayStatics::ApplyPointDamage(OtherActor, DamageAmount, SweepResult.Normal, SweepResult, NULL, DamageCauser, UDamageType::StaticClass());

}

void AEnemy::StartBinding()
{
	ServerStartBinding();
}

void AEnemy::StartPoison()
{
	bPoisoned = true;
	PoisonLimitTime = PoisonedTime;
}

void AEnemy::OnRep_Burning_Implementation()
{
	if (bBurning) BurningLimitTime = BurningTime;
}

void AEnemy::ServerApplyDamage_Implementation(AActor* OtherActor, float DamageAmount, AController* DamageInstigator, AActor* DamageCauser, TSubclassOf<UDamageType> DamageType)
{
	MultiApplyDamage(OtherActor, DamageAmount, DamageInstigator, DamageCauser, DamageType);
}

void AEnemy::MultiApplyDamage_Implementation(AActor* OtherActor, float DamageAmount, AController* DamageInstigator, AActor* DamageCauser, TSubclassOf<UDamageType> DamageType)
{
	UGameplayStatics::ApplyDamage(OtherActor, DamageAmount, DamageInstigator, DamageCauser, DamageType);
}

void AEnemy::ServerShowDamage_Implementation(float DamageAmount, const FVector& ShowLoc)
{
	MultiShowDamage(DamageAmount, ShowLoc);
}

void AEnemy::MultiShowDamage_Implementation(float DamageAmount, const FVector& ShowLoc)
{
	OnDamage.Broadcast(DamageAmount, ShowLoc);
}

void AEnemy::SeeNewTarget(AActor* NewTarget)
{
	ABaseCharacter* NewBaseCharacter = Cast<ABaseCharacter>(NewTarget);
	TargetArray.AddUnique(NewBaseCharacter);

	if (Target == NULL)
	{
		SetTarget(NewBaseCharacter);
		//CurState = EEnemyState::Chase;
		//EnemyAnim->StopCombatMontage(0.2f);
	}

}

void AEnemy::HearSound(FVector SoundLoc, AActor* AgroActor)
{
	//TargetPos = SoundLoc;
	//AgroTarget = AgroActor;
	//bWaitingAgro = true;
}

void AEnemy::ComeBackHome(float Distance)
{
	Target = NULL;
	if (Distance <= 100.f)
	{
		CurState = EEnemyState::Patrol;
	}
	else
	{
		CurState = EEnemyState::Return;
	}
}

bool AEnemy::CheckInMoveRange()
{
	float Distance = FVector::Dist2D(BaseLocation, GetActorLocation());
	bool bResult = Distance <= MoveRange;
	if (bResult) return true;
	else
	{
		ComeBackHome(Distance);
		return false;
	}
}

void AEnemy::LeaveTarget(AActor* KilledTarget)
{
	for (int i = 0; i < TargetArray.Num(); ++i)
	{
		if (TargetArray[i] == KilledTarget)
		{
			TargetArray.RemoveAt(i);
		}
	}

	if (KilledTarget == Target)
	{
		Target = NULL;
		ChangeTarget();
	}

}

void AEnemy::ChangeTarget()
{
	int NearestTargetNum = 0;
	int ShortestDistance = 100000000;
	for (int i = 0; i < TargetArray.Num(); ++i)
	{
		if (TargetArray[i]->HP > 0)
		{
			FVector DirectionVec = TargetArray[i]->GetActorLocation() - GetActorLocation();
			DirectionVec.Z = 0;
			float Distance = DirectionVec.Size();

			if (Distance < ShortestDistance)
			{
				ShortestDistance = Distance;
				NearestTargetNum = i;
			}
		}
	}
	if (TargetArray.IsEmpty()) Target = NULL;
	else
	{
		Target = TargetArray[NearestTargetNum];
	}
}

int AEnemy::CheckInRange()
{
	return 0;
}

int AEnemy::CheckPattern()
{
	return 0;
}

void AEnemy::Attack(int AttackPattern)
{
	if (Target != NULL)
	{
		CurState = EEnemyState::NormalAttack;
		ServerPlayMontage(this, FName("Attack"));
	}
}

void AEnemy::PatternAttack(int AttackPattern)
{
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
	//ServerPlayMontage(this, FName("Attack_Jump"));
}

void AEnemy::ChargeAttack()
{
	CurState = EEnemyState::ChargeAttack;
	ServerPlayMontage(this, FName("ChargeAttack"));
}


void AEnemy::RushAttack()
{
	ServerPlayMontage(this, FName("Slide_Start"));
	CurState = EEnemyState::RushAttack;
}

void AEnemy::WideAttack()
{
	ServerPlayMontage(this, FName("WideAttack"));
	CurState = EEnemyState::WideAttack;

}

void AEnemy::LaunchToTarget()
{
	if (Target)
	{
		FVector Velocity = FVector::ZeroVector;
		FVector EndPos = Target->GetActorLocation();
		FVector StartPos = GetActorLocation();
		StartPos.Z = EndPos.Z;
		FVector LookVec = GetActorForwardVector();

		EndPos = EndPos - LookVec * GetCapsuleComponent()->GetScaledCapsuleRadius();
		float Speed = 600.f;

		bool JumpResult = false;
		while (!JumpResult)
		{
			//개선 필요
			JumpResult = UGameplayStatics::SuggestProjectileVelocity(this, Velocity, StartPos, EndPos,
				Speed, false, 0.f, GetWorld()->GetGravityZ(), ESuggestProjVelocityTraceOption::DoNotTrace);
			Speed += 100.f;

		}
		LaunchCharacter(Velocity, true, true);
	}
}
void AEnemy::Block()
{
	bReflecting = true;
}

void AEnemy::Reflect()
{
	//반격 시 변수 초기화
	FVector ExplosionLocation = GetActorLocation();
	float DamageRadius = 500.f;
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);
	UGameplayStatics::ApplyRadialDamage(GetWorld(), ReflectDamgeAmount, ExplosionLocation, DamageRadius, UDamageType::StaticClass(), IgnoreActors, GetController(), false);

	UE_LOG(LogTemp, Warning, TEXT("%f"), ReflectDamgeAmount);

	bReflecting = false;
	ReflectDamgeAmount = 0.f;
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
	if (HasAuthority())
	{
		ServerSpawnItemBox(SpawnLoc, DropItemBoxClass, DropItemID_Array);
	}
	/*AItemDropActor* ItemBox = GetWorld()->SpawnActor<AItemDropActor>(DropItemBoxClass, SpawnLoc, GetActorRotation());
	ItemBox->CreateItemArray(DropItemID_Array);*/
}

void AEnemy::ServerSpawnItemBox_Implementation(const FVector& SpawnLoc, TSubclassOf<AInteractActor> SpawnClass, const TArray<FName>& ItemID_Array)
{
	AItemDropActor* ItemBox = GetWorld()->SpawnActor<AItemDropActor>(DropItemBoxClass, SpawnLoc, GetActorRotation());
	ItemBox->CreateItemArray(ItemID_Array);
}

void AEnemy::ServerSpawnProjectile_Implementation(TSubclassOf<class AEnemyProjectile>SpawnClass, const FVector& SpawnLoc, const FVector& EndLoc, const FVector& DirectionVec)
{
	AEnemyProjectile* Projectile = GetWorld()->SpawnActor<AEnemyProjectile>(SpawnClass, SpawnLoc, DirectionVec.Rotation());
	Projectile->FirstUse(DirectionVec, SpawnLoc, EndLoc);
}

void AEnemy::ServerDestroy_Implementation()
{
	MultiDestroy();
}

void AEnemy::MultiDestroy_Implementation()
{
	Destroy();
}

void AEnemy::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!bInterrupted)
	{
		EnemyAnim->bPlaying = false;
		OnMontageEnd.Broadcast();
		CurState = EEnemyState::Chase;
		//if (CurState == EEnemyState::Hit) CurState = EEnemyState::Roar;

	}


}

void AEnemy::InteractIce_Implementation()
{
	if (bSlow) return;
	GetCharacterMovement()->MaxWalkSpeed /= 2;
	bSlow = true;
	SlowLimitTime = SlowTime;
}

void AEnemy::InteractFire_Implementation(UPrimitiveComponent* HitComponent)
{
	if (bSlow)
	{
		return;
	}

	bBurning = true;
	BurningLimitTime = BurningTime;
}

void AEnemy::InteractBindTrap_Implementation()
{
	bBinding = true;
	StartBindingTime = GetWorld()->TimeSeconds;
}

void AEnemy::InteractEarthquake_Implementation()
{
}

void AEnemy::InteractAttack_Implementation(FVector HitDirection, float Damage)
{
	if (Damage <= 0.f)
	{
		return;
	}
	if (HitDirection.Z < 0.f) HitDirection.Z *= -1;

	LaunchCharacter(HitDirection * 1000.f, false, false);
}

void AEnemy::InteractGrabAttack_Implementation()
{
}

void AEnemy::InteractIceSkill_Implementation()
{
	if (bFrozen) return;
	else if (bSlow)
	{
		bFrozen = true;
		FrozenLimitTime = FrozenTime;
	}

	else if (bBurning)
	{
		bWeaken = true;
		bBurning = false;
		BurningLimitTime = 0.f;
	}
}
