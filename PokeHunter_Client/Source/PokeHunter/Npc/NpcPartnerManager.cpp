// Fill out your copyright notice in the Description page of Project Settings.


#include "NpcPartnerManager.h"
#include "../Partner/Partner.h"
#include "../Partner/PartnerController.h"

ANpcPartnerManager::ANpcPartnerManager()
{
	CameraBoom->TargetArmLength = 0.f;
	static ConstructorHelpers::FClassFinder<UUserWidget> TempUI(TEXT("/Game/UI/PartnerMgr/WBP_PartnerMangerUI"));
	if (TempUI.Succeeded())
	{
		UIClass = TempUI.Class;
	}

	for (int i = 0; i < 3; ++i)
	{
		PositionArray.AddDefaulted();
		PartnerArray.AddDefaulted();
	}
}

void ANpcPartnerManager::BeginPlay()
{
	FVector BaseLocation = GetActorLocation();
	float LookVecX = GetActorForwardVector().X;
	for (int i = 0; i<3; ++i)
	{
		PositionArray[i] = FVector(BaseLocation.X - LookVecX * 400.f, BaseLocation.Y + PARTNER_GAP * (i - 1), BaseLocation.Z);
		APartner* temp = GetWorld()->SpawnActor<APartner>(PartnerClassArray[i], PositionArray[i], GetActorRotation());
		temp->TargetPos = temp->GetActorLocation();
		temp->LookTargetVec = GetActorForwardVector();
		PartnerArray[i] = temp;
	}
	PosePos = FVector(BaseLocation.X - LookVecX * 200.f, BaseLocation.Y, BaseLocation.Z);
}

void ANpcPartnerManager::Tick(float DeltaTime)
{

}

void ANpcPartnerManager::Interact_Implementation(AHunter* Hunter)
{
	ANpc::Interact_Implementation(Hunter);
	

}

void ANpcPartnerManager::LeavePartner()
{
	for (int i = 0; i < 3; ++i)
	{
		if (PartnerArray[i] == NULL)
		{
			PartnerArray[i] = Master->Partner;
			APartner* SelectedPartner = PartnerArray[i];
			SelectedPartner->Hunter = NULL;
			SelectedPartner->TargetPos = PositionArray[i];
			Master->Partner = NULL;
			break;
		}
	}
}

void ANpcPartnerManager::ChangePartner()
{
	if(Master->Partner)
	UE_LOG(LogTemp, Warning, TEXT("Change success"));
	APartner* SelectedPartner = PartnerArray[CurrentIndex];
	SelectedPartner->TargetPos = PosePos;
	SelectedPartner->bPosing = true;
	SelectedPartner->CurState = EPartnerState::Posing;
}

void ANpcPartnerManager::AddIndex(int Val)
{
	if (Val < 0 && CurrentIndex == 0) return;
	else if (Val > 0 && CurrentIndex == PartnerArray.Num()-1) return;

	PartnerArray[CurrentIndex]->TargetPos = PositionArray[CurrentIndex];
	PartnerArray[CurrentIndex]->bPosing = false;
	CurrentIndex += Val;
}

void ANpcPartnerManager::ShowPartner(int Val)
{
	PartnerArray[CurrentIndex]->TargetPos = PositionArray[CurrentIndex];
	PartnerArray[CurrentIndex]->bPosing = false;
	CurrentIndex = Val;
}

void ANpcPartnerManager::SelectPartner()
{
	PartnerArray[CurrentIndex]->FollowHunter(Master);
	Master->SetPartner(PartnerArray[CurrentIndex]);

	PartnerArray[CurrentIndex] = NULL;
}
