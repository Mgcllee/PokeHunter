// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_EnemyTransform.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "GameFramework/Character.h"

UBTTask_EnemyTransform::UBTTask_EnemyTransform()
{
}

EBTNodeResult::Type UBTTask_EnemyTransform::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	Character = Cast<ACharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (!Character) return EBTNodeResult::Failed;

	return EBTNodeResult::InProgress;
}

void UBTTask_EnemyTransform::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{

	Character->GetMesh()->SetRelativeLocation(FVector(0, 0, -30));
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}
