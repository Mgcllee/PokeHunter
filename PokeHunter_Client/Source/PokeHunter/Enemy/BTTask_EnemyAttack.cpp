// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_EnemyAttack.h"
#include "Enemy.h"
#include "EnemyAnimInstance.h"
#include "EnemyController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_EnemyAttack::UBTTask_EnemyAttack()
{
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_EnemyAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
	Enemy = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	UEnemyAnimInstance* EnemyAnim = Enemy->EnemyAnim;
	if (Enemy == NULL)return EBTNodeResult::Failed;
	if(Enemy->CurState == EEnemyState::NormalAttack) return EBTNodeResult::Failed;
	int PatternNum = OwnerComp.GetBlackboardComponent()->GetValueAsInt(FName("AttackPattern"));
	Enemy->Attack(PatternNum);
	bPlaying = true;

	Enemy->OnMontageEnd.AddLambda([this]()->void
		{
			bPlaying = false;
		});

	return EBTNodeResult::Type::InProgress;
}

void UBTTask_EnemyAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	if (!bPlaying)
	{
		Enemy->CurState = EEnemyState::Chase;
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		
	}
}



