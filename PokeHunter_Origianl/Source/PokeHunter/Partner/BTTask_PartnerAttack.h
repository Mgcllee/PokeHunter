// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_PartnerAttack.generated.h"

/**
 * 
 */
UCLASS()
class POKEHUNTER_API UBTTask_PartnerAttack : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_PartnerAttack();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
	void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	class AWolfPartner* Wolf;
	bool bPlaying{ false };
};
