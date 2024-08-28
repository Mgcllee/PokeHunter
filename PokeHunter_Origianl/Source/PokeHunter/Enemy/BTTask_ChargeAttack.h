// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ChargeAttack.generated.h"

/**
 * 
 */
UCLASS()
class POKEHUNTER_API UBTTask_ChargeAttack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	bool bPlaying{ false };

public:
	UBTTask_ChargeAttack();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
	void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
};
