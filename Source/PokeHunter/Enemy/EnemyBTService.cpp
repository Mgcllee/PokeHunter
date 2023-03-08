// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBTService.h"
#include "Enemy.h"
#include "EnemyController.h"
#include "BehaviorTree/BlackboardComponent.h"

void UEnemyBTService::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AEnemy* Enemy = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	OwnerComp.GetBlackboardComponent()->SetValueAsObject(FName("Target"), Enemy->Target);
	OwnerComp.GetBlackboardComponent()->SetValueAsEnum(FName("State"), (uint8)Enemy->CurState);
}
