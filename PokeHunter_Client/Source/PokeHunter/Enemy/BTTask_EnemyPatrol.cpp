// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_EnemyPatrol.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemy.h"
#include "EnemyAnimInstance.h"
#include "EnemyController.h"

UBTTask_EnemyPatrol::UBTTask_EnemyPatrol()
{
	NodeName = TEXT("Patrol");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_EnemyPatrol::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	Enemy = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	if (Enemy == NULL) return EBTNodeResult::Failed;

	bPlaying = true;

	//Patrol
	Enemy->Patrol();

	Enemy->OnMontageEnd.AddLambda([this]() -> void {
		bPlaying = false;
		});


	return EBTNodeResult::InProgress;
}

void UBTTask_EnemyPatrol::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	AEnemyController* Controller = Cast<AEnemyController>(OwnerComp.GetAIOwner());
	//Patrol 종료 조건
	if (Controller->FindAgroActor() == true)
	{

		Enemy->CurState = EEnemyState::Chase;
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}


}
