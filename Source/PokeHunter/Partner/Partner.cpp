// Fill out your copyright notice in the Description page of Project Settings.


#include "Partner.h"
#include "PartnerController.h"
#include "PartnerAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "PokeHunter/Hunter/Hunter.h"

// Sets default values
APartner::APartner()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TeamID = FGenericTeamId(0);

	GetCharacterMovement()->bOrientRotationToMovement = true;

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

}

// Called to bind functionality to input
void APartner::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

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
		PartnerAnim->PlayCombatMontage(TEXT("Attack"));
	}


	
}

void APartner::Howling()
{
	//Play Montage
	PartnerAnim->PlayCombatMontage(TEXT("Howling"));
}

void APartner::StopSkill()
{
	bOrdered = false;
	bUsingSkill = false;
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
}

void APartner::UseNormalSkill(ESkillID SkillID)
{
	//��ų ����� ���� �Ұ� �߰� �ʿ�

	if (bUsingSkill) return;
	bOrdered = true;
	bUsingSkill = true;

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

void APartner::UseSpecialSkill(ESkillID SkillID)
{
	//��ų ����� ���� �Ұ� �߰� �ʿ�

	if (bUsingSkill) return;
	bOrdered = true;
	bUsingSkill = true;

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

void APartner::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	PartnerAnim->bPlaying = false;

	OnMontageEnd.Broadcast();

}
