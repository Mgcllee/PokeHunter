// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_EnemySpecialAttack.generated.h"

/**
 * 
 */
UCLASS()
class POKEHUNTER_API UBTTask_EnemySpecialAttack : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_EnemySpecialAttack();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
	void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

public:
	class AEnemy* Enemy;
	bool bPlaying{ false };
};
