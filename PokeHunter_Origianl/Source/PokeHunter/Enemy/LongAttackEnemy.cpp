// Fill out your copyright notice in the Description page of Project Settings.


#include "LongAttackEnemy.h"
#include "EnemyProjectile.h"
#include "PokeHunter/Base/BaseCharacter.h"

ALongAttackEnemy::ALongAttackEnemy()
{
	// GetCapsuleComponent()->SetCapsuleRadius(90.f);
	// GetCapsuleComponent()->SetGenerateOverlapEvents(true);


}

void ALongAttackEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateOverlaps(true);
}

void ALongAttackEnemy::LaunchProjectile()
{
	if (Target)
	{
		FVector TargetLoc = Target->GetActorLocation();

		//Socket 변환 필요
		FVector CurLoc = GetActorLocation();
		FVector DirectionVec = TargetLoc - CurLoc;
		DirectionVec.Normalize();

		if (ProjectileClass)
		{
			ServerSpawnProjectile(ProjectileClass, CurLoc, TargetLoc, DirectionVec);

			//AEnemyProjectile* ProjectileActor =  GetWorld()->SpawnActor<AEnemyProjectile>(ProjectileClass, GetActorTransform());
			//ProjectileActor->FireInDirection(DirectionVec);
		}

	}
}
