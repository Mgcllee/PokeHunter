// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_JumpAttack.h"
#include "Enemy.h"
#include "EnemyController.h"

UBTTask_JumpAttack::UBTTask_JumpAttack()
{
	
}

EBTNodeResult::Type UBTTask_JumpAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
	AEnemy* Enemy = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	if (Enemy == NULL) return EBTNodeResult::Failed;
	if(Enemy->CurState == EEnemyState::JumpAttack) return EBTNodeResult::Failed;
	Enemy->JumpAttack();

	bPlaying = true;

	Enemy->OnMontageEnd.AddLambda([this]()->void
		{
			bPlaying = false;
		});

	bNotifyTick = true;
	return EBTNodeResult::InProgress;
}

void UBTTask_JumpAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AEnemy* Enemy = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	if (!Enemy->IsJumping() && !bPlaying)
	{
		Enemy->CurState = EEnemyState::Chase;
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}

}
