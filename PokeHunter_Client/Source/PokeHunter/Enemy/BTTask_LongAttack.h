// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_LongAttack.generated.h"

/**
 * 
 */
UCLASS()
class POKEHUNTER_API UBTTask_LongAttack : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_LongAttack();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
	void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	bool bPlaying{ false };
};
