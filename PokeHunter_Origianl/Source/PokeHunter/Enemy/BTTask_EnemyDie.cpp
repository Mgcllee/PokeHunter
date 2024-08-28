// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_EnemyDie.h"
#include "Enemy.h"
#include "EnemyController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_EnemyDie::UBTTask_EnemyDie()
{
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_EnemyDie::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
	Enemy = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	return EBTNodeResult::InProgress;
}

void UBTTask_EnemyDie::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	FVector CurScale = Enemy->GetActorScale();
	Enemy->SetActorScale3D(CurScale - DeltaSeconds * FVector(1.f,1.f,1.f));
	if (Enemy->GetActorScale().Length() < 0.2f)
	{
		Enemy->Die();
		//FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
