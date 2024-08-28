// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_PartnerHowling.h"
#include "Partner.h"
#include "PartnerController.h"

UBTTask_PartnerHowling::UBTTask_PartnerHowling()
{
	bNotifyTick = true;
	NodeName = TEXT("Howling");
}

EBTNodeResult::Type UBTTask_PartnerHowling::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	//일정 범위 안의 enemy 클래스들에게 어그로 끌기
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
	APartner* Partner = Cast<APartner>(OwnerComp.GetAIOwner()->GetPawn());
	if (Partner == NULL)return EBTNodeResult::Failed;
	if(Partner->CurState != EPartnerState::Howling) return EBTNodeResult::Failed;
	Partner->Howling();
	bPlaying = true;

	Partner->OnMontageEnd.AddLambda([this]()->void
		{
			bPlaying = false;
		});


	return EBTNodeResult::Type::InProgress;
}

void UBTTask_PartnerHowling::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (!bPlaying)
	{
		APartner* Partner = Cast<APartner>(OwnerComp.GetAIOwner()->GetPawn());
		Partner->StopSkill();
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
