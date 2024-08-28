// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_EnemyRushAttack.h"
#include "Enemy.h"
#include "EnemyController.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTTask_EnemyRushAttack::UBTTask_EnemyRushAttack()
{

}

EBTNodeResult::Type UBTTask_EnemyRushAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
	Enemy = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	if (Enemy == NULL) return EBTNodeResult::Failed;
	if (Enemy->CurState == EEnemyState::JumpAttack) return EBTNodeResult::Failed;
	Enemy->RushAttack();
	Enemy->GetCharacterMovement()->MaxWalkSpeed = 1000.f;

	bPlaying = true;

	Enemy->OnMontageEnd.AddLambda([this]()->void
		{
			bPlaying = false;
		});

	bNotifyTick = true;
	return EBTNodeResult::InProgress;

	return EBTNodeResult::Type();
}

void UBTTask_EnemyRushAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	float Distance = Enemy->GetDistanceTo(Enemy->Target);
	if (!bSliding && Distance <= 1000.f)
	{
		bSliding = true;
		Enemy->TargetPos = Enemy->Target->GetActorLocation();
		Enemy->ServerPlayMontage(Enemy, FName("Slide_Jump"));
	}
	if (bSliding)
	{
		if (Enemy->GetVelocity().Length() <= 0.f)
		{
			Enemy->ServerPlayMontage(Enemy, FName("Slide_End"));
			if (!bPlaying)
			{
				Enemy->GetCharacterMovement()->MaxWalkSpeed = 500.f;
				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			}
		}
	}
}
