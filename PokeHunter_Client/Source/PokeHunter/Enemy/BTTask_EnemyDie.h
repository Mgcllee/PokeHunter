// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_EnemyDie.generated.h"

/**
 * 
 */
UCLASS()
class POKEHUNTER_API UBTTask_EnemyDie : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	class AEnemy* Enemy;

public:
	UBTTask_EnemyDie();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
	void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
