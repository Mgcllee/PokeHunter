// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_EnemyPlayMontage.generated.h"

/**
 * 
 */
UCLASS()
class POKEHUNTER_API UBTTask_EnemyPlayMontage : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_EnemyPlayMontage();
	void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;


	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

public:
	class AEnemy* Enemy;
	bool bPlayOnce;
	bool bPlaying;
};
