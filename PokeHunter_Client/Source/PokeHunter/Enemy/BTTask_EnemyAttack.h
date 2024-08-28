// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_EnemyAttack.generated.h"

/**
 * 
 */
UCLASS()
class POKEHUNTER_API UBTTask_EnemyAttack : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_EnemyAttack();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

public:
	class AEnemy* Enemy;
	bool bPlaying{ false };
};
