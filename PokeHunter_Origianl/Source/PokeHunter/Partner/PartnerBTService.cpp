// Fill out your copyright notice in the Description page of Project Settings.


#include "PartnerBTService.h"
#include "PartnerController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "PokeHunter/Hunter/Hunter.h"
#include "Partner.h"

void UPartnerBTService::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APartner* Partner = Cast<APartner>(OwnerComp.GetAIOwner()->GetPawn());
	OwnerComp.GetBlackboardComponent()->SetValueAsObject(FName("Hunter"),Partner->Hunter);
	OwnerComp.GetBlackboardComponent()->SetValueAsVector(FName("TargetPos"), Partner->TargetPos);
	OwnerComp.GetBlackboardComponent()->SetValueAsBool(FName("bPosing"), Partner->bPosing);
	OwnerComp.GetBlackboardComponent()->SetValueAsBool(FName("bOrdered"), Partner->bOrdered);
	OwnerComp.GetBlackboardComponent()->SetValueAsEnum(FName("PartnerState"), (uint8)Partner->CurState);
	OwnerComp.GetBlackboardComponent()->SetValueAsObject(FName("Target"), Partner->Target);
	OwnerComp.GetBlackboardComponent()->SetValueAsBool(FName("bDied"), Partner->bDied);
}