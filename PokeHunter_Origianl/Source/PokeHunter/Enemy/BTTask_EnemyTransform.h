// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_EnemyTransform.generated.h"

/*
 * 
 */
UCLASS()
class POKEHUNTER_API UBTTask_EnemyTransform : public UBTTaskNode
{
	GENERATED_BODY()

public:

	class ACharacter* Character;

	UBTTask_EnemyTransform();
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
	void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds);



};
