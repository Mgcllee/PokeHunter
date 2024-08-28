// Fill out your copyright notice in the Description page of Project Settings.


#include "PartyListUI.h"
#include "PokeHunter/Network/PartyServerManager.h"

void UPartyListUI::NativeConstruct()
{
	Super::NativeConstruct();
	gameinstance = Cast<UBaseInstance>(UGameplayStatics::GetGameInstance((GetWorld())));

	/*BufferPartyServerManager = UPartyServerManager::StaticClass();
	PartyServerManager = NewObject<UPartyServerManager>(this, BufferPartyServerManager, FName("PartyServerManager"));
	if (nullptr != PartyServerManager) {
		PartyServerManager->gameinstance = gameinstance;
		PartyServerManager->GetPartyList();
		GetWorld()->GetTimerManager().SetTimer(TH_PartyList, this, &UPartyListUI::GetPartyList, 0.1f, true);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Fail Craete partyServerManager..."));
	}*/
}

void UPartyListUI::NativeDestruct()
{
	/*GetWorld()->GetTimerManager().ClearTimer(TH_PartyList);
	PartyServerManager->gameinstance = nullptr;*/
	Super::NativeDestruct();
}

bool UPartyListUI::GetPartyList()
{
	if (0 == gameinstance->PartyListMap.Num()) return false;

	if (false == Name_PartyList.IsEmpty())	Name_PartyList.Empty();
	if (false == Count_PartyList.IsEmpty()) Count_PartyList.Empty();

	for (const auto& info : gameinstance->PartyListMap) {
		Name_PartyList.Add(info.Key);
	}

	for (const auto& info : gameinstance->PartyListMap) {
		Count_PartyList.Add(info.Value);
	}

	for (int32 info : gameinstance->inStageParty) {
		inStage_PartyList.Add(info);
	}

	return true;
}
