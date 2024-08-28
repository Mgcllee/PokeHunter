// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_WolfIceShard.generated.h"

/**
 * 
 */
UCLASS()
class POKEHUNTER_API UBTTask_WolfIceShard : public UBTTaskNode
{
	GENERATED_BODY()
	
	UBTTask_WolfIceShard();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
	void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	bool bPlaying{ false };

};
