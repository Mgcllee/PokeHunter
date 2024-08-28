// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Roar.h"
#include "Enemy.h"
#include "EnemyAnimInstance.h"
#include "EnemyController.h"

UBTTask_Roar::UBTTask_Roar()
{
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_Roar::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
	AEnemy* Enemy = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	UEnemyAnimInstance* EnemyAnim = Enemy->EnemyAnim;
	if (Enemy == NULL)return EBTNodeResult::Failed;
	if (Enemy->CurState != EEnemyState::Roar) return EBTNodeResult::Failed;
	bPlaying = true;

	
	Enemy->Roar();

	Enemy->OnMontageEnd.AddLambda([this]()->void
		{
			bPlaying = false;
		});

	 
	return EBTNodeResult::Type::InProgress;
}

void UBTTask_Roar::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	AEnemy* Enemy = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	if (!bPlaying)
	{
		Enemy->bWaitingAgro = false;
		Enemy->CurState = EEnemyState::Chase;
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		
	}
}