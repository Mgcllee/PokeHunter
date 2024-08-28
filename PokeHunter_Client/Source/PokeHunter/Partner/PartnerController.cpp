// Fill out your copyright notice in the Description page of Project Settings.


#include "PartnerController.h"
#include "Partner.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BehaviorTree.h"
#include "PokeHunter/Hunter/Hunter.h"

APartnerController::APartnerController()
{
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BBObject(TEXT("BlackboardData'/Game/Partner/BB_Partner.BB_Partner'"));
	if (BBObject.Succeeded())
	{
		BBPartner = BBObject.Object;
	}


	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTObject(TEXT("BehaviorTree'/Game/Partner/BT_Partner.BT_Partner'"));
	if (BTObject.Succeeded())
	{
		BTPartner = BTObject.Object;
	}
	
	
}

void APartnerController::OnPossess(APawn* pawn)
{
	Super::OnPossess(pawn);
	RunAI();
}

void APartnerController::RunAI()
{
	//Upgrade5
	//if (UseBlackboard(BBPartner, Blackboard)) RunBehaviorTree(BTPartner);

	RunBehaviorTree(BTPartner);

}

void APartnerController::StopAI()
{
	UBehaviorTreeComponent* BehaviorTreeComponent = Cast<UBehaviorTreeComponent>(BrainComponent);
	if (BehaviorTreeComponent == nullptr) return;
	BehaviorTreeComponent->StopTree(EBTStopMode::Safe);
}