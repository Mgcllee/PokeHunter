// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_EnemyDetect.h"
#include "Enemy.h"
#include "EnemyController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTService_EnemyDetect::UBTService_EnemyDetect()
{
	NodeName = TEXT("DetectTarget");
}

void UBTService_EnemyDetect::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AEnemy* Enemy = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	if (Enemy == NULL) return;

	UWorld* World = Enemy->GetWorld();
	FVector EnemyLoc = Enemy->GetActorLocation();

	float AttackRange = Enemy->AttackRange;

	Enemy->Target->GetActorLocation();

	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams CollisionParam(NAME_None, false, Enemy);
	bool bResult = World->OverlapMultiByChannel(
		OverlapResults,
		EnemyLoc,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel1,
		FCollisionShape::MakeSphere(AttackRange),
		CollisionParam
	);

	if (bResult)
	{
		for (const FOverlapResult& OverlapResult : OverlapResults)
		{

		}
	}

}
