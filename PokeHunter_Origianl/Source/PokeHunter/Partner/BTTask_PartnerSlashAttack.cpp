// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_PartnerSlashAttack.h"
#include "Partner.h"
#include "PartnerAnimInstance.h"
#include "PartnerController.h"

UBTTask_PartnerSlashAttack::UBTTask_PartnerSlashAttack()
{
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_PartnerSlashAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
	APartner* Partner = Cast<APartner>(OwnerComp.GetAIOwner()->GetPawn());
	if (Partner == NULL) return EBTNodeResult::Failed;
	if (Partner->CurState != EPartnerState::SlashAttack)  return EBTNodeResult::Failed;
	Partner->SlashAttack();
	bPlaying = true;

	Partner->OnMontageEnd.AddLambda([this]()->void
		{
			bPlaying = false;
		});

	return EBTNodeResult::Type::InProgress;
}

void UBTTask_PartnerSlashAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (!bPlaying)
	{
		APartner* Partner = Cast<APartner>(OwnerComp.GetAIOwner()->GetPawn());
		Partner->StopSkill();
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}

}
