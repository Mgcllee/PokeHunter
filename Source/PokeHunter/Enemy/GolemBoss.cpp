// Fill out your copyright notice in the Description page of Project Settings.


#include "GolemBoss.h"
#include "Components/BoxComponent.h"
#include "PokeHunter/Base/HitBoxComponent.h"
#include "PokeHunter/Item/Item.h"
#include "PokeHunter/Hunter/Hunter.h"
#include "EnemyAnimInstance.h"
#include "EnemyProjectile.h"
#include "Kismet/GameplayStatics.h"

AGolemBoss::AGolemBoss()
{
	FName PartName = FName("HeadSocket");
	HeadHitBox = CreateDefaultSubobject<UHitBoxComponent>(FName("HeadHitBox"));
	HeadHitBox->SetupAttachment(GetMesh(), PartName);
	HeadHitBox->SetCollisionProfileName(FName("EnemyHitBox"));
	HitBoxMap.Add(PartName, HeadHitBox);

	PartName = FName("Body");
	BodyHitBox = CreateDefaultSubobject<UHitBoxComponent>(FName("BodyHitBox"));
	BodyHitBox->SetupAttachment(GetMesh(), PartName);
	BodyHitBox->SetCollisionProfileName(FName("EnemyHitBox"));
	HitBoxMap.Add(PartName, BodyHitBox);

	PartName = FName("RightHand");
	RightHandHitBox = CreateDefaultSubobject<UHitBoxComponent>(FName("RightHandHitBox"));
	RightHandHitBox->SetupAttachment(GetMesh(), PartName);
	RightHandHitBox->SetCollisionProfileName(FName("EnemyHitBox"));
	HitBoxMap.Add(PartName, RightHandHitBox);

	PartName = FName("RightArm");
	RightArmHitBox = CreateDefaultSubobject<UHitBoxComponent>(FName("RightArmHitBox"));
	RightArmHitBox->SetupAttachment(GetMesh(), PartName);
	RightArmHitBox->SetCollisionProfileName(FName("EnemyHitBox"));
	RightArmHitBox->SetChild(RightHandHitBox);
	HitBoxMap.Add(PartName, RightArmHitBox);

	PartName = FName("RightShoulder");
	RightShoulderHitBox = CreateDefaultSubobject<UHitBoxComponent>(FName("RightShoulderHitBox"));
	RightShoulderHitBox->SetupAttachment(GetMesh(), PartName);
	RightShoulderHitBox->SetCollisionProfileName(FName("EnemyHitBox"));
	RightShoulderHitBox->SetChild(RightArmHitBox);
	HitBoxMap.Add(PartName, RightShoulderHitBox);

	PartName = FName("LeftHand");
	LeftHandHitBox = CreateDefaultSubobject<UHitBoxComponent>(FName("LeftHandHitBox"));
	LeftHandHitBox->SetupAttachment(GetMesh(), PartName);
	LeftHandHitBox->SetCollisionProfileName(FName("EnemyHitBox"));
	HitBoxMap.Add(PartName, LeftHandHitBox);

	PartName = FName("LeftArm");
	LeftArmHitBox = CreateDefaultSubobject<UHitBoxComponent>(FName("LeftArmHitBox"));
	LeftArmHitBox->SetupAttachment(GetMesh(), PartName);
	LeftArmHitBox->SetCollisionProfileName(FName("EnemyHitBox"));
	LeftArmHitBox->SetChild(LeftHandHitBox);
	HitBoxMap.Add(PartName, LeftArmHitBox);

	PartName = FName("RightLeg");
	RightLegHitBox = CreateDefaultSubobject<UHitBoxComponent>(FName("RightLegHitBox"));
	RightLegHitBox->SetupAttachment(GetMesh(), PartName);
	RightLegHitBox->SetCollisionProfileName(FName("EnemyHitBox"));
	HitBoxMap.Add(PartName, RightLegHitBox);

	PartName = FName("LeftLeg");
	LeftLegHitBox = CreateDefaultSubobject<UHitBoxComponent>(FName("LeftLegHitBox"));
	LeftLegHitBox->SetupAttachment(GetMesh(), PartName);
	LeftLegHitBox->SetCollisionProfileName(FName("EnemyHitBox"));
	LeftLegHitBox->SetIsReplicated(true);
	HitBoxMap.Add(PartName, LeftLegHitBox);


	AttackRange = 1000.f;

	//Earthquake Collision
	EarthquakeCollision = CreateDefaultSubobject<UStaticMeshComponent>(FName("EarthquakeCollision"));
	EarthquakeCollision->SetupAttachment(GetRootComponent());
	EarthquakeCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);



}

void AGolemBoss::Tick(float DeltaTime)
{
	APawn::Tick(DeltaTime);

	//for (int i = 0; i < PartHitBoxs.Num() - 1; ++i)
	//{
	//	if (PartHitBoxs[i]->CheckBurning(DeltaTime))
	//	{
	//		float DamageAmount = 1;
	//		HP -= DamageAmount;
	//		OnDamage.Broadcast(DamageAmount, PartHitBoxs[i]->GetComponentLocation());
	//	}
	//}

	for (auto Hitbox : HitBoxMap)
	{
		if (!Hitbox.Value)
		{
			if (Hitbox.Value->CheckBurning(DeltaTime))
			{
				float DamageAmount = 1;
				HP -= DamageAmount;
				OnDamage.Broadcast(DamageAmount, Hitbox.Value->GetComponentLocation());
			}
		}
	}
}

void AGolemBoss::BeginPlay()
{
	Super::BeginPlay();

	//for (int i = 0; i < PartHitBoxs.Num() - 1; ++i)
	//{
	//	PartHitBoxs[i]->OnComponentBeginOverlap.AddDynamic(this, &AGolemBoss::OnOverlapBegin);
	//	PartHitBoxs[i]->BurningTime = BurningTime;
	//}

	for (auto Hitbox : HitBoxMap)
	{
		Hitbox.Value->OnComponentBeginOverlap.AddDynamic(this, &AGolemBoss::OnOverlapBegin);
		Hitbox.Value->BurningTime = BurningTime;
	}

	isDie = false;
}

void AGolemBoss::Die()
{
	TArray<USceneComponent*> ChildComponents;
	GetMesh()->GetChildrenComponents(true, ChildComponents);
	for (auto Component : ChildComponents)
	{
		auto PrimitiveComponent = Cast<UPrimitiveComponent>(Component);
		PrimitiveComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		PrimitiveComponent->SetSimulatePhysics(true);
		PrimitiveComponent->SetCollisionProfileName(FName("Destructible"));
		PrimitiveComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}

	
	isDie = true;
}

void AGolemBoss::LongAttack()
{
	if (EnemyAnim)
	{
		CurState = EEnemyState::LongAttack;
		MultiPlayMontage(this, FName("Throw"));
		
	}
	TargetPos = Target->GetActorLocation();
}

void AGolemBoss::LaunchStone()
{
	FVector InitialPos = GetMesh()->GetSocketLocation(FName("LeftHand"));
	if (Target == NULL) UE_LOG(LogTemp, Warning, TEXT("Dd"));
	FVector EndPos = Target->GetActorLocation();
	FVector DirectionVec = EndPos - InitialPos;
	DirectionVec.Normalize();


	AEnemyProjectile* Stone = GetWorld()->SpawnActor<AEnemyProjectile>(ProjectileClass, InitialPos, DirectionVec.Rotation());
	Stone->FireInDirection(DirectionVec, InitialPos, EndPos);
}

void AGolemBoss::InteractFire_Implementation(UPrimitiveComponent* HitComponent)
{
	auto* HitBox = Cast<UHitBoxComponent>(HitComponent);
	if (HitBox)
	{
		HitBox->bBurning = true;
		HitBox->StartBurningTime = GetWorld()->TimeSeconds;
	}
}

void AGolemBoss::OpenLevelHome()
{
	FString LevelName = GetWorld()->GetName();
	if ("L_Field0" == LevelName && "MyHome" != LevelName) {
		FString levelName = L"/Game/Map/Lobby/MyHome";
		UGameplayStatics::OpenLevel(GetWorld(), *levelName);
	}
}


float AGolemBoss::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (HP <= 0) return 0;
	APawn::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	

	FVector HitLoc;
	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{	//Point Damage

		const FPointDamageEvent& PointDamageEvent = static_cast<const FPointDamageEvent&>(DamageEvent);
		HitLoc = PointDamageEvent.HitInfo.Location;
		UHitBoxComponent* HitBox = Cast<UHitBoxComponent>(PointDamageEvent.HitInfo.GetComponent());
		if (HitBox)
		{
			if (HitBox->TakeDamage(DamageAmount))
			{
				FName PartName = HitBox->GetAttachSocketName();
				DestroyPart(PartName);
				HitBox->ServerDestroyPart();
			}
		}
		HP -= DamageAmount;
	}
	else
	{
		//�Ϲ� ������ ó��
		HP -= DamageAmount;
		HitLoc = GetActorLocation();
	}

	if (HP <= 0)
	{
		ServerPlayMontage(this, FName("Die"));
		bDied = true;
		SetActorTickEnabled(false);
		//Die();
	}

	if (bReflecting)
	{	//�ݰ� ���� ������ ����
		ReflectDamgeAmount += DamageAmount;
	}

	OnDamage.Broadcast(DamageAmount, HitLoc);

	return DamageAmount;
}

void AGolemBoss::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UHitBoxComponent* OverlapHitBox = Cast<UHitBoxComponent>(OverlappedComp);
	 
	if (OverlapHitBox)
	{
		if (bCanGrab)
		{
			FName PartName = OverlapHitBox->GetAttachSocketName();
			if (PartName == FName("LeftHand") || PartName == FName("RightHand"))
			{
				ACharacter* GrabbedCharacter = Cast<ACharacter>(OtherActor);
				if (GrabbedCharacter)
				{
					OtherActor->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("GrabSocket"));
					GrabbedCharacter->SetActorEnableCollision(false);
					GrabbedTarget = GrabbedCharacter;
				}
				return;
			}
		}

		if (IEnemyInteractInterface* EnemyInteractInterface = Cast<IEnemyInteractInterface>(OtherActor))
		{
			FVector HitDirection = OtherActor->GetActorLocation() - OverlappedComp->GetComponentLocation();
			HitDirection.Normalize();
			EnemyInteractInterface->Execute_InteractAttack(OtherActor, HitDirection, OverlapHitBox->Damage);
		}

		UGameplayStatics::ApplyPointDamage(OtherActor, OverlapHitBox->Damage, SweepResult.Normal, SweepResult, GetController(), this, UDamageType::StaticClass());

	}
}

void AGolemBoss::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	UHitBoxComponent* HitBox = Cast<UHitBoxComponent>(HitComponent);
	if (HitBox)
	{
		if (bCanGrab)
		{
			FName PartName = HitBox->GetAttachSocketName();
			if (PartName == FName("LeftHand") || PartName == FName("RightHand"))
			{
				ACharacter* GrabbedCharacter = Cast<ACharacter>(OtherActor);
				if (GrabbedCharacter)
				{
					OtherActor->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("GrabSocket"));
					GrabbedCharacter->SetActorEnableCollision(false);
					GrabbedTarget = GrabbedCharacter;
				}
				return;
			}
		}

		if (IEnemyInteractInterface* EnemyInteractInterface = Cast<IEnemyInteractInterface>(OtherActor))
		{
			
			EnemyInteractInterface->Execute_InteractAttack(OtherActor, Hit.Normal, HitBox->Damage);


		}

		UGameplayStatics::ApplyPointDamage(OtherActor, HitBox->Damage, Hit.ImpactNormal, Hit, GetController(), this, UDamageType::StaticClass());

	}
}

void AGolemBoss::DestroyPart_Implementation(FName PartName)
{

}

void AGolemBoss::DeleteHitBox(FName PartName)
{
	HitBoxMap.Remove(PartName);
}

void AGolemBoss::Attack(int AttackPattern)
{
	switch (AttackPattern)
	{
	case 0:
		if (Target != NULL)
		{
			ServerPlayMontage(this, FName("Attack"));
			//EnemyAnim->PlayCombatMontage(TEXT("Attack"));
		}
		break;
	case 1:
		ServerPlayMontage(this, FName("Attack_Punch"));
		break;

	case 2:
		ServerPlayMontage(this, FName("Attack_Grab"));
		break;

	case 3:
		ServerPlayMontage(this, FName("Attack_Bind"));
		break;
	case 4:
		ServerPlayMontage(this, FName("Block"));
		Block();
		break;
	}
}

void AGolemBoss::Earthquake()
{
	FHitResult HitResult;
	FVector LocVec = GetActorLocation();
	FVector DownVec = (-1) * GetActorUpVector();
	float HalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	FVector StartTrace = LocVec + DownVec * HalfHeight;

		TArray<AActor*> OverlapActors;
		EarthquakeCollision->GetOverlappingActors(OverlapActors);
		for (auto OverlapActor : OverlapActors)
		{
			IEnemyInteractInterface* ApplyActor = Cast<IEnemyInteractInterface>(OverlapActor);
			if (ApplyActor)
			{
				Execute_InteractEarthquake(OverlapActor);

			}
		}
}