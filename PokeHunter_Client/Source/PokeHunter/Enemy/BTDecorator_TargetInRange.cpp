// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_TargetInRange.h"
#include "Enemy.h"
#include "EnemyController.h"
#include "components/CapsuleComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "PokeHunter/Base/BaseCharacter.h"


bool UBTDecorator_TargetInRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	AEnemy* Enemy = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	if (Enemy == NULL) return false;
	ABaseCharacter* CurTarget = Cast<ABaseCharacter>(Enemy->Target);
	if (CurTarget == NULL) return false;

	if (CurTarget->HP <= 0)
	{
		Enemy->ChangeTarget();
	}
	float AttackRange = Enemy->AttackRange;
	float Radius = Enemy->GetCapsuleComponent()->GetUnscaledCapsuleRadius();
	float TargetRadius = CurTarget->GetCapsuleComponent()->GetUnscaledCapsuleRadius();
	float Distance = CurTarget->GetDistanceTo(Enemy) - TargetRadius;
	bResult = (Distance <= AttackRange);
	if (bResult)
	{
		int PatternNum = Enemy->CheckInRange();
		OwnerComp.GetBlackboardComponent()->SetValueAsInt(FName("AttackPattern"), PatternNum);
	}

	else
	{
		int PatternNum = Enemy->CheckPattern();
		OwnerComp.GetBlackboardComponent()->SetValueAsInt(FName("AttackPattern"), PatternNum);
	}

	return bResult;	
}
