// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_IceBreathe.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "WolfPartner.h"
#include "PartnerController.h"

UBTTask_IceBreathe::UBTTask_IceBreathe()
{
	bNotifyTick = true;
	NodeName = TEXT("IceBreathe");
}

EBTNodeResult::Type UBTTask_IceBreathe::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
	AWolfPartner* Wolf = Cast<AWolfPartner>(OwnerComp.GetAIOwner()->GetPawn());
	if (Wolf == NULL)return EBTNodeResult::Failed;
	if (Wolf->CurState != EPartnerState::IceBreath) return EBTNodeResult::Failed;
	bPlaying = true;
	Wolf->IceBreathe();

	Wolf->OnMontageEnd.AddLambda([this]()->void
		{
			bPlaying = false;
		});

	return EBTNodeResult::Type::InProgress;
}

void UBTTask_IceBreathe::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{

	APawn* Pawn = Cast<APawn>(OwnerComp.GetAIOwner()->GetPawn());
	if (Pawn == nullptr) FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	APartner* Partner = Cast<APartner>(Pawn);
	FVector TargetPos = Partner->Target->GetActorLocation();
	FVector LookVec = TargetPos - Pawn->GetActorLocation();

	

	LookVec.Z = 0.f;
	FVector ForwardVec = Pawn->GetActorForwardVector();

	LookVec.Normalize();
	FRotator Rot = FRotationMatrix::MakeFromX(LookVec).Rotator();

	Pawn->SetActorRotation(FMath::RInterpTo(Pawn->GetActorRotation(), Rot, GetWorld()->GetDeltaSeconds(), 8.f));


	if (!bPlaying)
	{
		Partner->StopSkill();
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
