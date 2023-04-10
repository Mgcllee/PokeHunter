// Fill out your copyright notice in the Description page of Project Settings.


#include "LongAttackEnemy.h"
#include "EnemyProjectile.h"

void ALongAttackEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

void ALongAttackEnemy::LaunchProjectile()
{
	if (Target)
	{
		FVector TargetLoc = Target->GetActorLocation();

		//Socket ��ȯ �ʿ�
		FVector CurLoc = GetActorLocation();
		FVector DirectionVec = TargetLoc - CurLoc;
		DirectionVec.Normalize();

		if (ProjectileClass)
		{
			AEnemyProjectile* ProjectileActor =  GetWorld()->SpawnActor<AEnemyProjectile>(ProjectileClass, GetActorTransform());
			ProjectileActor->FireInDirection(DirectionVec);

		}

	}
}
