// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_TurnToTarget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "PokeHunter/Enemy/Enemy.h"
#include "Partner.h"
#include "PartnerController.h"

UBTTask_TurnToTarget::UBTTask_TurnToTarget()
{
	bNotifyTick = true;
	NodeName = TEXT("TurnToTarget");
}

EBTNodeResult::Type UBTTask_TurnToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	return EBTNodeResult::InProgress;
}


void UBTTask_TurnToTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* Pawn = Cast<APawn>(OwnerComp.GetAIOwner()->GetPawn());
	if (Pawn == nullptr) FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	FVector TargetPos = OwnerComp.GetBlackboardComponent()->GetValueAsVector(TEXT("TargetPos"));
	FVector LookVec = TargetPos - Pawn->GetActorLocation();
	
	APartner* Partner =Cast<APartner>(Pawn);
	AEnemy* Enemy = Cast<AEnemy>(Pawn);
	if (Partner)
	{
		if (Partner->CurState == EPartnerState::Posing || Partner->CurState == EPartnerState::Unselected)
		{
			LookVec = Partner->LookTargetVec;
		}

		else if (Partner->CurState == EPartnerState::Unselected)
		{
			LookVec = Partner->LookTargetVec;
		}
	}

	else if (Enemy)
	{
		if (Enemy->Target != NULL)
		{
			TargetPos = Enemy->Target->GetActorLocation();
			LookVec = TargetPos - Pawn->GetActorLocation();
		}
	}

	LookVec.Z = 0.f;
	FVector ForwardVec = Pawn->GetActorForwardVector();

	LookVec.Normalize();
	FRotator Rot = FRotationMatrix::MakeFromX(LookVec).Rotator();

	Pawn->SetActorRotation(FMath::RInterpTo(Pawn->GetActorRotation(), Rot, GetWorld()->GetDeltaSeconds(), 5.f));



	float BetweenAngle = FMath::Acos(FVector::DotProduct(LookVec, ForwardVec) / (LookVec.Size() * ForwardVec.Size())) * (180.0f / PI);
	if (BetweenAngle < 10.f)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}