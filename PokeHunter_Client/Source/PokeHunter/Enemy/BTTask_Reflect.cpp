// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Reflect.h"
#include "Enemy.h"
#include "EnemyController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_Reflect::UBTTask_Reflect()
{
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_Reflect::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
	Enemy = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	UEnemyAnimInstance* EnemyAnim = Enemy->EnemyAnim;
	if (Enemy == NULL) return EBTNodeResult::Failed;
	if (Enemy->CurState == EEnemyState::LongAttack)  return EBTNodeResult::Failed;
	Enemy->Reflect();
	bPlaying = true;

	Enemy->OnMontageEnd.AddLambda([this]()->void
		{
			bPlaying = false;
		});

	return EBTNodeResult::InProgress;
}

void UBTTask_Reflect::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	if (!bPlaying)
	{
		Enemy->CurState = EEnemyState::Chase;
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
