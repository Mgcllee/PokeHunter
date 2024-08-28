// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_PartnerAttack.h"
#include "WolfPartner.h"
#include "PartnerController.h"

UBTTask_PartnerAttack::UBTTask_PartnerAttack()
{
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_PartnerAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
	Wolf = Cast<AWolfPartner>(OwnerComp.GetAIOwner()->GetPawn());
	if (Wolf == NULL)return EBTNodeResult::Failed;
	if (Wolf->CurState != EPartnerState::MakingStorm) return EBTNodeResult::Failed;
	Wolf->MakeStorm();
	bPlaying = true;

	Wolf->OnMontageEnd.AddLambda([this]()->void
		{
			bPlaying = false;
		});

	return EBTNodeResult::Type::InProgress;
}

void UBTTask_PartnerAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (!bPlaying)
	{
		APartner* Partner = Cast<APartner>(OwnerComp.GetAIOwner()->GetPawn());
		Partner->StopSkill();
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

