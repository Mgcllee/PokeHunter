// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_PartnerKnockback.h"
#include "Partner.h"
#include "PartnerController.h"

UBTTask_PartnerKnockback::UBTTask_PartnerKnockback()
{

}

EBTNodeResult::Type UBTTask_PartnerKnockback::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
	 Partner = Cast<APartner>(OwnerComp.GetAIOwner()->GetPawn());
	if (Partner == NULL) return EBTNodeResult::Failed;
	bNotifyTick = true;

	return EBTNodeResult::InProgress;
}

void UBTTask_PartnerKnockback::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (!Partner->CheckFalling())
	{
		Partner->CurState = EPartnerState::MoveTarget;
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}

}
