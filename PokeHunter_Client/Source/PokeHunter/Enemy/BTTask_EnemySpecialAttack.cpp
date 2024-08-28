// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_EnemySpecialAttack.h"
#include "Enemy.h"
#include "EnemyAnimInstance.h"
#include "EnemyController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_EnemySpecialAttack::UBTTask_EnemySpecialAttack()
{
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_EnemySpecialAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
	Enemy = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	UEnemyAnimInstance* EnemyAnim = Enemy->EnemyAnim;
	if (Enemy == NULL)return EBTNodeResult::Failed;
	if (Enemy->CurState == EEnemyState::PatternAttack) return EBTNodeResult::Failed;
	int PatternNum = OwnerComp.GetBlackboardComponent()->GetValueAsInt(FName("AttackPattern"));
	Enemy->PatternAttack(PatternNum);
	bPlaying = true;

	Enemy->OnMontageEnd.AddLambda([this]()->void
		{
			bPlaying = false;
		});

	return EBTNodeResult::Type::InProgress;
}

void UBTTask_EnemySpecialAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	if (!bPlaying && !Enemy->IsJumping())
	{
		Enemy->CurState = EEnemyState::Chase;
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);

	}
}
