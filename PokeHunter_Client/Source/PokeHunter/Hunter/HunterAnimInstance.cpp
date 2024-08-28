// Fill out your copyright notice in the Description page of Project Settings.


#include "HunterAnimInstance.h"
#include "Hunter.h"

UHunterAnimInstance::UHunterAnimInstance()
{

}

void UHunterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	::UAnimInstance::NativeUpdateAnimation(DeltaSeconds);

	auto Pawn = TryGetPawnOwner();
	if (::IsValid(Pawn)) 
	{
		MovementSpeed = Pawn->GetVelocity().Size();
	}
}

void UHunterAnimInstance::NativeInitializeAnimation()
{
	::UAnimInstance::NativeInitializeAnimation();

	if (Hunter == nullptr)
	{
		Hunter = Cast<AHunter>(TryGetPawnOwner());
	}
}

void UHunterAnimInstance::UpdateAnimationProperties()
{
	if(Hunter != nullptr)
	{
		FVector Speed = Hunter->GetVelocity();
		FVector XYspeed = FVector(Speed.X, Speed.Y, 0.f);
		MovementSpeed = XYspeed.Size();

		XYspeed.Normalize();
		Direction = FMath::Atan2(Speed.Y, Speed.X) * 180.f / PI;

		//Upper
		bUpperOnly = Hunter->bUpperOnly;

		//Falling
		bFalling = Hunter->GetCharacterMovement()->IsFalling();
	}
}

void UHunterAnimInstance::PlayCombatMontage(FName Section)
{	
	float PlayRate = 1.0f;
	float MaxSpeed = 700.f;
	//if (Section == FName("Dive"))
	//{
	//	PlayRate = 0.14f;
	//	PlayRate += MovementSpeed / MaxSpeed;
	//	Hunter->SetDiveCurveTime(PlayRate);
	//}
	Montage_Play(CombatMontage,PlayRate);
	Montage_JumpToSection(Section, CombatMontage);
}

void UHunterAnimInstance::PlayInteractMontage(FName Section)
{
	Montage_Play(InteractMontage, 1.0f);
	Montage_JumpToSection(Section, InteractMontage);
}
