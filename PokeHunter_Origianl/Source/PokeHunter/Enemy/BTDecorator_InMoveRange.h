// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_InMoveRange.generated.h"

/**
 * 
 */
UCLASS()
class POKEHUNTER_API UBTDecorator_InMoveRange : public UBTDecorator
{
	GENERATED_BODY()
	
	bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const;
};
