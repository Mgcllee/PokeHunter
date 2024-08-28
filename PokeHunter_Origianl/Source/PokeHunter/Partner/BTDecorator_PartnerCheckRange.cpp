// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_PartnerCheckRange.h"
#include "Partner.h"
#include "PartnerController.h"

bool UBTDecorator_PartnerCheckRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	APartner* Partner = Cast<APartner>(OwnerComp.GetAIOwner()->GetPawn());
	if (Partner == NULL) return false;

	if (Partner->Target == NULL)
		return false;

	bResult = (Partner->Target->GetDistanceTo(Partner) <= 200);
	return bResult;
}
