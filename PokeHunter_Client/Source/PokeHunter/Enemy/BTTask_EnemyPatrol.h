// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_EnemyPatrol.generated.h"

/**
 * 
 */
UCLASS()
class POKEHUNTER_API UBTTask_EnemyPatrol : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_EnemyPatrol();

	void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;


	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

public:
	class AEnemy* Enemy;
	bool bPlaying;
};
