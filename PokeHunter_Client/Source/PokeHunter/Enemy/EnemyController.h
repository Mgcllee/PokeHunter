// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"

#include "EnemyController.generated.h"

/**
 * 
 */
UCLASS()
class POKEHUNTER_API AEnemyController : public AAIController
{
	GENERATED_BODY()

public:
	AEnemyController();

	virtual void BeginPlay();

	virtual void OnPossess(APawn* pawn) override;

	UFUNCTION()
	void OnPerception(AActor* Actor, FAIStimulus Stimulus);

	UFUNCTION(Server, Reliable)
	void RunAI();
	UFUNCTION(Server, Reliable)
	void StopAI();
	bool FindAgroActor();

public:
	/*UPROPERTY(VisibleAnywhere, BlueprintReadWrite, "AI")
	class UBehaviorTreeComponent* BTComponent;*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI")
	class UBehaviorTree* BTEnemy;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI")
	class UBlackboardData* BBEnemy;
	UPROPERTY()
	class UAISenseConfig_Sight* ConfigSight;
	UPROPERTY()
	class UAISenseConfig_Hearing* ConfigHearing;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class AEnemy* Enemy;

};
