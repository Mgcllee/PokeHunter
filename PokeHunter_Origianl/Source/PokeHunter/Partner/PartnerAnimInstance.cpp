// Fill out your copyright notice in the Description page of Project Settings.


#include "PartnerAnimInstance.h"
#include "Partner.h"
#include "PokeHunter/Hunter/Hunter.h"

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
		Target = Partner->Target;
		Hunter = Partner->Hunter;

		//Target Loc
		if (Partner->CurState == EPartnerState::Unselected) bLookAt = false;
		else
		{
			if (Target)
			{
				FVector LookVec = Partner->GetActorForwardVector();
				FVector DirectionVec = Target->GetActorLocation() - Partner->GetActorLocation();
				DirectionVec.Normalize();
				float DotProduct = FVector::DotProduct(LookVec, DirectionVec);
				float Yaw = FMath::RadiansToDegrees(FMath::Atan2(DirectionVec.Y, DirectionVec.X) - FMath::Atan2(LookVec.Y, LookVec.X));
				float Pitch = FMath::RadiansToDegrees(FMath::Atan2(DirectionVec.Z, FMath::Sqrt(DirectionVec.X * DirectionVec.X + DirectionVec.Y * DirectionVec.Y))
					- FMath::Atan2(LookVec.Z, FMath::Sqrt(LookVec.X * LookVec.X + LookVec.Y * LookVec.Y)));

				if (Yaw <= 60.f && Yaw >= -60.f && Pitch <= 60.f && Pitch >= -60.f)
				{
					bLookAt = true;
					TargetLoc = Target->GetActorLocation();
				}
			}
			else if (Hunter)
			{
				FVector LookVec = Partner->GetActorForwardVector();
				FVector DirectionVec = Hunter->GetActorLocation() - Partner->GetActorLocation();
				DirectionVec.Normalize();
				float DotProduct = FVector::DotProduct(LookVec, DirectionVec);
				float Yaw = FMath::RadiansToDegrees(FMath::Atan2(DirectionVec.Y, DirectionVec.X) - FMath::Atan2(LookVec.Y, LookVec.X));
				float Pitch = FMath::RadiansToDegrees(FMath::Atan2(DirectionVec.Z, FMath::Sqrt(DirectionVec.X * DirectionVec.X + DirectionVec.Y * DirectionVec.Y))
					- FMath::Atan2(LookVec.Z, FMath::Sqrt(LookVec.X * LookVec.X + LookVec.Y * LookVec.Y)));

				if (Yaw <= 60.f && Yaw >= -60.f && Pitch <= 60.f && Pitch >= -60.f)
				{
					bLookAt = true;
					TargetLoc = Hunter->GetActorLocation();
				}
			}
			else bLookAt = false;
		}
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

