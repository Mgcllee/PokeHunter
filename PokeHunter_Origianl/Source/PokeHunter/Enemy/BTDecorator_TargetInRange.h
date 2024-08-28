// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_TargetInRange.generated.h"

/**
 * 
 */
UCLASS()
class POKEHUNTER_API UBTDecorator_TargetInRange : public UBTDecorator
{
	GENERATED_BODY()

	bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const;

};
