// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnimInstance.h"
#include "Enemy.h"

UEnemyAnimInstance::UEnemyAnimInstance()
{

	MovementSpeed = 0.0f;
}

void UEnemyAnimInstance::UpdateAnimationProperties()
{

	if (Enemy == nullptr)
	{
		Enemy = Cast<AEnemy>(TryGetPawnOwner());
	}

	else
	{
		Enemy->GetVelocity();
		FVector Speed = Enemy->GetVelocity();
		FVector XYspeed = FVector(Speed.X, Speed.Y, 0.f);
		MovementSpeed = XYspeed.Size();
		bJumping = Enemy->IsJumping();

		XYspeed.Normalize();
		Direction = FMath::Atan2(Speed.Y, Speed.X) * 180.f / PI;

		//Target Loc
		AActor* Target = Enemy->Target;
		if (Enemy->bDied) bLookAt = false;
		else
		{
			if (Target)
			{
				FVector LookVec = Enemy->GetActorForwardVector();
				FVector DirectionVec = Target->GetActorLocation() - Enemy->GetActorLocation();
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
			else bLookAt = false;
		}
	}
}

void UEnemyAnimInstance::StopCombatMontage(float BlendTime)
{
	Montage_Stop(BlendTime, CombatMontage);
}

bool UEnemyAnimInstance::PlayCombatMontage(FName Section, bool bInterrupt)
{
	//if (Section == FName("Die") || Section == FName("Hit"))

	if (CurSection == FName("Die")) return false;
	else if (CurSection == FName("Hit") && Section != FName("Die")) return false;
	else if (CurSection == FName("LeftDestroy") || CurSection == FName("RightDestroy"))
	{
		if (Section != FName("LeftDestroy") && Section != FName("RightDestroy")) return false;
	}

	if(bInterrupt)
	{
		
		if (Section == FName("Patrol"))
		{
			float StartTime = FMath::RandRange(13.f,20.f);
			bPlaying = true;
			Montage_Play(CombatMontage, 1.0f);
			Montage_SetPosition(CombatMontage, StartTime);
		}
		else
		{
			bPlaying = true;
			Montage_Play(CombatMontage, 1.0f);
			Montage_JumpToSection(Section, CombatMontage);
		}
		

		return true;
	}
	else if (!bPlaying)
	{
		if (Section == FName("Patrol"))
		{
			float StartTime = FMath::RandRange(13.f,20.f);
			bPlaying = true;
			Montage_Play(CombatMontage, 1.0f);
			Montage_SetPosition(CombatMontage, StartTime);
		}

		else
		{
			bPlaying = true;
			Montage_Play(CombatMontage, 1.0f);
			Montage_JumpToSection(Section, CombatMontage);
		}
		return true;
	}
	return false;
}
