// Fill out your copyright notice in the Description page of Project Settings.


#include "GolemPartner.h"
#include "GameFramework/CharacterMovementComponent.h"

AGolemPartner::AGolemPartner()
{
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
}

void AGolemPartner::UseSpecialSkill(ESkillID SkillID)
{
	bOrdered = true;
	TargetPos = Target->GetActorLocation();
	switch (SkillID)
	{
	case ESkillID::RockThrow:
		ThrowRock();
		break;

	case ESkillID::Meteor:
		UseMeteor();
		break;

	default:
		break;
	}
}

void AGolemPartner::UseMeteor()
{

}

void AGolemPartner::ThrowRock()
{

}
