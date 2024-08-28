// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "PartnerController.generated.h"

/**
 * 
 */
UCLASS()
class POKEHUNTER_API APartnerController : public AAIController
{
	GENERATED_BODY()

public:
	APartnerController();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class AHunter* Hunter;

	virtual void OnPossess(APawn* pawn) override;

	void RunAI();
	void StopAI();

public:
	UPROPERTY()
		class UBehaviorTree* BTPartner;
	UPROPERTY()
		class UBlackboardData* BBPartner;

	
};
