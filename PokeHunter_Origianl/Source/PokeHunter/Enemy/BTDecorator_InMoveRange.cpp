// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_InMoveRange.h"
#include "BehaviorTree/BTDecorator.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemy.h"
#include "EnemyController.h"

bool UBTDecorator_InMoveRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	AEnemy* Enemy = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	if (Enemy == NULL) return false;

	float Distance = FVector::Dist2D(Enemy->BaseLocation, Enemy->GetActorLocation());

	bResult = (Distance <= Enemy->MoveRange);
	OwnerComp.GetBlackboardComponent()->SetValueAsBool(FName("bInMoveRange"), bResult);

	return bResult;
}
