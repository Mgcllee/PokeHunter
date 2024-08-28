// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Roar.generated.h"

/**
 * 
 */
UCLASS()
class POKEHUNTER_API UBTTask_Roar : public UBTTaskNode
{
	GENERATED_BODY()
public:

	UBTTask_Roar();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
	void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
public:
	UBehaviorTreeComponent* BTComponent;
	bool bPlaying{ true };
};
