// Fill out your copyright notice in the Description page of Project Settings.


#include "PartnerAnimInstance.h"
#include "Partner.h"

UPartnerAnimInstance::UPartnerAnimInstance()
{
	MovementSpeed = 0.0f;
}

void UPartnerAnimInstance::NativeInitializeAnimation()
{
	if (Partner == nullptr)
	{
		Partner = Cast<APartner>(TryGetPawnOwner());
	}
	
}

void UPartnerAnimInstance::UpdateAnimationProperties()
{
	if (Partner == nullptr)
	{
		Partner = Cast<APartner>(TryGetPawnOwner());
	}
	if (Partner != nullptr)
	{
		FVector Speed = Partner->GetVelocity();
		FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.f);
		MovementSpeed = LateralSpeed.Size();
	}
}

bool UPartnerAnimInstance::PlayCombatMontage(FName Section)
{
	if (!bPlaying)
	{
		bPlaying = true;
		Montage_Play(CombatMontage, 1.0f);
		Montage_JumpToSection(Section, CombatMontage);
		return true;
	}
	return false;
}

