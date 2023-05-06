// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PartyInfoUI.h"

#include "WebBrowser.h"
#include "WebBrowserModule.h"
#include "IWebBrowserSingleton.h"
#include "IWebBrowserCookieManager.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

UPartyInfoUI::UPartyInfoUI(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SearchingForGame = false;

	AveragePlayerLatency = 60.0;
}

void UPartyInfoUI::NativeConstruct()
{
	Super::NativeConstruct();

	gameinstance = Cast<UBaseInstance>(UGameplayStatics::GetGameInstance((GetWorld())));
	SendEnterParty();
	// TickSendPartyInfo();
	GetWorld()->GetTimerManager().SetTimer(TH_Partyinfo, this, &UPartyInfoUI::TickSendPartyInfo, 1.0f, true, 0.f);

	// Join Blueprint UI widget �߰� �ʿ�
	JoinButton = (UButton*)GetWidgetFromName(TEXT("BTN_READY"));
	FScriptDelegate JoinDelegate{};
	JoinDelegate.BindUFunction(this, "OnJoinButtonClicked");
	JoinButton->OnClicked.Add(JoinDelegate);

	// TB_JoinEvent (TextBlock user Widget �߰� �ʿ�!)
	JoinEventTextBlock = (UTextBlock*)GetWidgetFromName(TEXT("TB_JoinEvent"));

	GetWorld()->GetTimerManager().SetTimer(SetAveragePlayerLatencyHandle, this, &UPartyInfoUI::SetAveragePlayerLatency, 1.0f, true, 1.0f);

	FString AccessToken;
	UGameInstance* GameInstance = GetGameInstance();
	UBaseInstance* PokeHunterGameInstance = nullptr;
	if (GameInstance != nullptr) {
		PokeHunterGameInstance = Cast<UBaseInstance>(GameInstance);
		if (PokeHunterGameInstance != nullptr) {
			AccessToken = PokeHunterGameInstance->AccessToken;

			// Id Access�� �ƴ� Access Token�� ���ȴ�.
			// AccessToken = PokeHunterGameInstance->IdToken;
		}
	}

	// AccessToken �� ��ȿ�� ���
	if (AccessToken.Len() > 0 && nullptr != PokeHunterGameInstance) {
		TSharedRef<IHttpRequest> GetPlayerDataRequest = gameinstance->HttpModule->CreateRequest();
		GetPlayerDataRequest->OnProcessRequestComplete().BindUObject(this, &UPartyInfoUI::OnGetPlayerDataResponseReceived);
		GetPlayerDataRequest->SetURL(PokeHunterGameInstance->ApiUrl + "/getplayerdata");
		GetPlayerDataRequest->SetVerb("GET");
		
		// GetPlayerDataRequest->SetHeader("Content-Type", "application/json");
		
		GetPlayerDataRequest->SetHeader("Authorization", AccessToken);
		GetPlayerDataRequest->ProcessRequest();
	}
}

void UPartyInfoUI::NativeDestruct()
{
	GetWorld()->GetTimerManager().ClearTimer(TH_Partyinfo);
	GetWorld()->GetTimerManager().ClearTimer(TH_WaitOtherClient);
	GetWorld()->GetTimerManager().ClearTimer(PollMatchmakingHandle);
	GetWorld()->GetTimerManager().ClearTimer(SetAveragePlayerLatencyHandle);
	Super::NativeDestruct();
}

bool UPartyInfoUI::SendEnterParty()
{
	if (nullptr == gameinstance)																	return false;
	if (ESocketConnectionState::SCS_NotConnected == gameinstance->gSocket->GetConnectionState())	return false;
	if (ESocketConnectionState::SCS_ConnectionError == gameinstance->gSocket->GetConnectionState()) return false;
	if (0 == gameinstance->PartyListMap.Num())														return false;
	if (-1 == SelctPartyNumber)																		return false;

	bool retVal = false;
	int32 bSize;

	CS_PARTY_ENTER_PACK enter_pack;
	enter_pack.size = sizeof(CS_PARTY_ENTER_PACK);
	enter_pack.type = CS_PARTY_ENTER;
	enter_pack.party_num = SelctPartyNumber;
	retVal = gameinstance->gSocket->Send(reinterpret_cast<const uint8*>(&enter_pack), enter_pack.size, bSize);
	if (false == retVal) return false;

	SC_PARTY_ENTER_OK_PACK ok_pack;
	retVal = gameinstance->gSocket->Recv(reinterpret_cast<uint8*>(&ok_pack), sizeof(SC_PARTY_ENTER_OK_PACK), bSize);
	if (false == retVal) return false;

	FString AccessToken;
	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance != nullptr) {
		UBaseInstance* PokeHunterGameInstance = Cast<UBaseInstance>(GameInstance);
		if (PokeHunterGameInstance != nullptr) {
			AccessToken = PokeHunterGameInstance->AccessToken;

			if (AccessToken.Len() > 0) {
				TSharedPtr<FJsonObject> PartyNumberObj = MakeShareable(new FJsonObject);

				// Send SelctPartyNumber 
				PartyNumberObj->SetNumberField("PartyNumber", SelctPartyNumber + 1);

				FString RequestBody;
				TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
				if (FJsonSerializer::Serialize(PartyNumberObj.ToSharedRef(), Writer)) {
					TSharedRef<IHttpRequest> StartJoinRequest = gameinstance->HttpModule->CreateRequest();
					StartJoinRequest->SetURL(PokeHunterGameInstance->ApiUrl + "/setpartynumber");
					StartJoinRequest->SetVerb("POST");
					StartJoinRequest->SetHeader("Content-Type", "application/json");
					StartJoinRequest->SetHeader("Authorization", AccessToken);
					StartJoinRequest->SetContentAsString(RequestBody);
					StartJoinRequest->ProcessRequest();
				}
			}
		}
	}

	return true;
}

void UPartyInfoUI::TickSendPartyInfo()	// Request Client -> Server
{
	if (nullptr == gameinstance)																	return;
	if (ESocketConnectionState::SCS_NotConnected == gameinstance->gSocket->GetConnectionState())	return;
	if (ESocketConnectionState::SCS_ConnectionError == gameinstance->gSocket->GetConnectionState()) return;
	if (0 == gameinstance->PartyListMap.Num())														return;
	if (-1 == SelctPartyNumber)																		return;

	// ���ο� ��Ƽ ������ ���ؼ� ������ ������ ���� �����.
	if(false == PlayerName.IsEmpty())		PlayerName.Empty();
	if(false == PlayerPetName.IsEmpty())	PlayerPetName.Empty();
	if(false == PartyMemberState.IsEmpty())	PartyMemberState.Empty();

	bool retVal = false;
	int32 bSize;
	
	CS_PARTY_INFO_PACK quest_pack;
	quest_pack.size = sizeof(CS_PARTY_INFO_PACK);
	quest_pack.type = CS_PARTY_INFO;
	quest_pack.party_num = SelctPartyNumber;
	retVal = gameinstance->gSocket->Send(reinterpret_cast<const uint8*>(&quest_pack), quest_pack.size, bSize);
	if (false == retVal) return;

	SC_PARTY_INFO_PACK info_pack;
	for (int i = 0; i < 4; ++i) {	// �ִ� �÷��̾� �� ��ŭ�� �ݺ��Ѵ�.
		memset(&info_pack, 0, sizeof(SC_PARTY_INFO_PACK));
		retVal = gameinstance->gSocket->Recv(reinterpret_cast<uint8*>(&info_pack), sizeof(SC_PARTY_INFO_PACK), bSize);

		// ����/���� ������ ū ������ �˻�
		if (false == retVal)									return;	// ���� ����� ����
		if (0 == strcmp(info_pack._mem, "theEnd")) {
			TCHAR MBTWBuffer[128];
			memset(MBTWBuffer, NULL, 128);
			MultiByteToWideChar(CP_ACP, 0, (LPCSTR)info_pack._my_name, -1, MBTWBuffer, strlen(info_pack._my_name));
			gameinstance->MyName = MBTWBuffer;
			break;	// ��Ƽ��� ���� ����
		}
		if (i == 0 && (0 == strcmp(info_pack._mem, "Empty")))	break;	// ù ��° ������� �������
		if (i > 0 && (0 == strcmp(info_pack._mem, "Empty")))	break;	// �� ��° ��� ���� �������

		TCHAR MBTWBuffer[128];
		memset(MBTWBuffer, NULL, 128);
		MultiByteToWideChar(CP_ACP, 0, (LPCSTR)info_pack._mem, -1, MBTWBuffer, strlen(info_pack._mem));
		
		// ���� ��Ƽ ����� �غ���� ǥ�ÿ�
		PlayerName.Add(FName(MBTWBuffer));

		memset(MBTWBuffer, NULL, 128);
		MultiByteToWideChar(CP_ACP, 0, (LPCSTR)info_pack._mem_pet, -1, MBTWBuffer, strlen(info_pack._mem_pet));

		uint8 pet_number = atoi(TCHAR_TO_ANSI(MBTWBuffer));
		PlayerPetName.Add(EPartnerType(pet_number));		// �۵��� �ȵ� ���, TArray�� �ڷ����� ����
		PartyMemberState.Add(PLAYER_STATE(info_pack._mem_state));
	}

	gameinstance->PlayerName.Empty();
	gameinstance->PlayerPetName.Empty();
	gameinstance->PartyMemberState.Empty();

	gameinstance->PlayerName = PlayerName;
	gameinstance->PlayerPetName = PlayerPetName;
	gameinstance->PartyMemberState = PartyMemberState;

	return;
}

bool UPartyInfoUI::RecvClientJoin()	// [CheckPoint]-Blueprint���� ȣ��Ǵ� ���� �ֳ�? -> ������ ����
{
	if (nullptr == gameinstance)	return false;
	if (ESocketConnectionState::SCS_NotConnected == gameinstance->gSocket->GetConnectionState()) return false;
	if (ESocketConnectionState::SCS_ConnectionError == gameinstance->gSocket->GetConnectionState()) return false;
	if (-1 == SelctPartyNumber)		return false;
	bool retVal = false;
	int32 bSize{};

	/*SC_PARTY_JOIN_SUCCESS_PACK start_party_pack;
	retVal = gameinstance->gSocket->Recv(reinterpret_cast<uint8*>(&start_party_pack), sizeof(SC_PARTY_JOIN_SUCCESS_PACK), bSize);*/

	if (true == retVal) {
		// AWS ����
		return true;
	}
	else				return false;

	// Clinet���� ��� ���� ���
	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("[Recv] IOCP server...")));

	return true;
}

bool UPartyInfoUI::LeaveParty()
{
	bool retVal = false;
	int32 bSize;

	CS_PARTY_LEAVE_PACK leave_pack;
	leave_pack.size = sizeof(CS_PARTY_LEAVE_PACK);
	leave_pack.type = CS_PARTY_LEAVE;
	retVal = gameinstance->gSocket->Send(reinterpret_cast<const uint8*>(&leave_pack), leave_pack.size, bSize);

	SC_PARTY_LEAVE_SUCCESS_PACK ok_pack;
	ok_pack.size = sizeof(SC_PARTY_LEAVE_SUCCESS_PACK);
	ok_pack.type = SC_PARTY_LEAVE_SUCCESS;

	retVal = gameinstance->gSocket->Recv(reinterpret_cast<uint8*>(&ok_pack), sizeof(SC_PARTY_LEAVE_SUCCESS_PACK), bSize);
	if (false == retVal) {
		return false;
	}

	int32 leave_cnt = PlayerName.Find(FName(ok_pack._mem));

	if (-1 != leave_cnt) {
		PlayerName.RemoveAt(leave_cnt);
		PlayerPetName.RemoveAt(leave_cnt);
		PartyMemberState.RemoveAt(leave_cnt);

		gameinstance->PlayerName.Empty();
		gameinstance->PlayerPetName.Empty();
		gameinstance->PartyMemberState.Empty();

		gameinstance->PlayerName = PlayerName;
		gameinstance->PlayerPetName = PlayerPetName;
		gameinstance->PartyMemberState = PartyMemberState;

		GetWorld()->GetTimerManager().ClearTimer(TH_Partyinfo);
	}
	else {
		return false;
	}

	FString AccessToken;
	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance != nullptr) {
		UBaseInstance* PokeHunterGameInstance = Cast<UBaseInstance>(GameInstance);
		if (PokeHunterGameInstance != nullptr) {
			AccessToken = PokeHunterGameInstance->AccessToken;

			if (AccessToken.Len() > 0) {
				TSharedPtr<FJsonObject> PartyNumberObj = MakeShareable(new FJsonObject);

				// Send SelctPartyNumber 
				PartyNumberObj->SetNumberField("PartyNumber", 0);

				FString RequestBody;
				TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
				if (FJsonSerializer::Serialize(PartyNumberObj.ToSharedRef(), Writer)) {
					TSharedRef<IHttpRequest> StartJoinRequest = gameinstance->HttpModule->CreateRequest();
					StartJoinRequest->SetURL(PokeHunterGameInstance->ApiUrl + "/setpartynumber");
					StartJoinRequest->SetVerb("POST");
					StartJoinRequest->SetHeader("Content-Type", "application/json");
					StartJoinRequest->SetHeader("Authorization", AccessToken);
					StartJoinRequest->SetContentAsString(RequestBody);
					StartJoinRequest->ProcessRequest();
				}
			}
		}
	}

	return true;
}

void UPartyInfoUI::LoadingScreenCall()
{
	UE_LOG(LogTemp, Warning, TEXT("[Success] LoadingScreenCall() function called"));
}

void UPartyInfoUI::SetAveragePlayerLatency() {
	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance != nullptr) {
		UBaseInstance* PokeHunterGameInstance = Cast<UBaseInstance>(GameInstance);
		if (PokeHunterGameInstance != nullptr) {
			float TotalPlayerLatency = 0.0f;
			for (float PlayerLatency : PokeHunterGameInstance->PlayerLatencies) {
				TotalPlayerLatency += PlayerLatency;
			}

			if (TotalPlayerLatency > 0) {
				AveragePlayerLatency = TotalPlayerLatency / PokeHunterGameInstance->PlayerLatencies.Num();
			}
		}
	}
}

void UPartyInfoUI::OnJoinButtonClicked()
{
	WaitforOtherClient = true;
	UTextBlock* ButtonTextBlock = (UTextBlock*)JoinButton->GetChildAt(0);
	
	if (ButtonTextBlock->GetText().ToString().Equals(FString("Wait"))) {
		GetWorld()->GetTimerManager().ClearTimer(TH_WaitOtherClient);
		ButtonTextBlock->SetText(FText::FromString("READY"));
		// cancel match packet send and ...
		return;
	}

	JoinButton->SetIsEnabled(false);
	SendClientState();

	if (false == WaitforOtherClient) {
		GetWorld()->GetTimerManager().ClearTimer(TH_WaitOtherClient);
		ButtonTextBlock->SetText(FText::FromString("Entering"));
		JoinButton->SetIsEnabled(false);
		EnterStageMap();
		// BTN ��Ȱ��ȭ->Ȱ��ȭ ���ʿ�.
	}
	else {
		ButtonTextBlock->SetText(FText::FromString("Wait"));
		JoinButton->SetIsEnabled(false);
		GetWorld()->GetTimerManager().SetTimer(TH_WaitOtherClient, this, &UPartyInfoUI::TickRecvPartyState, 1.0f, true, 0.f);
	}
}

void UPartyInfoUI::TickRecvPartyState()
{
	SendClientState();

	if (false == WaitforOtherClient) {
		UTextBlock* ButtonTextBlock = (UTextBlock*)JoinButton->GetChildAt(0);
		ButtonTextBlock->SetText(FText::FromString("Entering"));
		JoinButton->SetIsEnabled(false);
		GetWorld()->GetTimerManager().ClearTimer(TH_Partyinfo);
		GetWorld()->GetTimerManager().ClearTimer(TH_WaitOtherClient);
		EnterStageMap();
	}
}

void UPartyInfoUI::SendClientState()	// Send this client State
{
	if (false == IsValid(gameinstance))																return;
	if (0 == gameinstance->PartyListMap.Num())														return;
	if (ESocketConnectionState::SCS_NotConnected == gameinstance->gSocket->GetConnectionState())	return;
	if (ESocketConnectionState::SCS_ConnectionError == gameinstance->gSocket->GetConnectionState()) return;

	bool retVal = false;
	int32 bSize;

	CS_PARTY_READY_PACK ready_pack;
	ready_pack.size = sizeof(CS_PARTY_READY_PACK);
	ready_pack.type = CS_PARTY_READY;
	retVal = gameinstance->gSocket->Send(reinterpret_cast<const uint8*>(&ready_pack), ready_pack.size, bSize);
	if (false == retVal) return;

	SC_PARTY_JOIN_RESULT_PACK result_pack;
	retVal = gameinstance->gSocket->Recv(reinterpret_cast<uint8*>(&result_pack), sizeof(SC_PARTY_JOIN_RESULT_PACK), bSize);
	if (false == retVal) return;

	if (-1 != result_pack._result) {	// Enter Server
		WaitforOtherClient = false;
	}
	else {								// Wait for other client
		WaitforOtherClient = true;
	}
	return;
}

void UPartyInfoUI::EnterStageMap()
{
	FString AccessToken;
	FString JoinTicketId;
	UGameInstance* GameInstance = GetGameInstance();
	UBaseInstance* PokeHunterGameInstance = nullptr;

	if (GameInstance != nullptr) {
		PokeHunterGameInstance = Cast<UBaseInstance>(GameInstance);
		if (PokeHunterGameInstance != nullptr) {
			// AWS Lambda StartMatchmaking Function �׽�Ʈ ��� Id Token�� �ƴ� Access Token�� �´�.
			// ->> �׷��� StartMatchmaking Function �ȿ��� GetPlayerData Function�� ������ ��, Id Token�� �ƴ� Access Token�� ����ϹǷ�
			// ->> API Gateway���� Startmachmaking API�� POST �κп��� Authorization ������ ������

			AccessToken = PokeHunterGameInstance->AccessToken;
			// AccessToken = PokeHunterGameInstance->IdToken;
			JoinTicketId = PokeHunterGameInstance->JoinTicketId;
		}
	}

	if (AccessToken.Len() > 0 && false == StartMath) {
		StartMath = true;

		TSharedRef<FJsonObject> LatencyMapObj = MakeShareable(new FJsonObject);
		LatencyMapObj->SetNumberField(PokeHunterGameInstance->RegionCode, AveragePlayerLatency);

		TSharedPtr<FJsonObject> RequestObj = MakeShareable(new FJsonObject);
		RequestObj->SetObjectField("latencyMap", LatencyMapObj);

		FString RequestBody;
		TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
		if (FJsonSerializer::Serialize(RequestObj.ToSharedRef(), Writer)) {
			TSharedRef<IHttpRequest> StartJoinRequest = gameinstance->HttpModule->CreateRequest();
			StartJoinRequest->OnProcessRequestComplete().BindUObject(this, &UPartyInfoUI::OnStartMatchmakingResponseReceived);
			StartJoinRequest->SetURL(PokeHunterGameInstance->ApiUrl + "/startmatchmaking");
			StartJoinRequest->SetVerb("POST");
			StartJoinRequest->SetHeader("Content-Type", "application/json");
			StartJoinRequest->SetHeader("Authorization", AccessToken);
			StartJoinRequest->SetContentAsString(RequestBody);
			StartJoinRequest->ProcessRequest();
		}
	}

	/*
	if (SearchingForGame) {
		GetWorld()->GetTimerManager().ClearTimer(PollMatchmakingHandle);
		SearchingForGame = false;

		// if (nullptr != PokeHunterGameInstance && AccessToken.Len() > 0 && JoinTicketId.Len() > 0) {
		if (AccessToken.Len() > 0 && JoinTicketId.Len() > 0) {
			TSharedPtr<FJsonObject> RequestObj = MakeShareable(new FJsonObject);
			RequestObj->SetStringField("ticketId", JoinTicketId);

			FString RequestBody;
			TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
			if (FJsonSerializer::Serialize(RequestObj.ToSharedRef(), Writer)) {
				TSharedRef<IHttpRequest> StopMatchmakingRequest = gameinstance->HttpModule->CreateRequest();
				StopMatchmakingRequest->OnProcessRequestComplete().BindUObject(this, &UPartyInfoUI::OnStopMatchmakingResponseReceived);
				StopMatchmakingRequest->SetURL(PokeHunterGameInstance->ApiUrl + "/stopmatchmaking");
				StopMatchmakingRequest->SetVerb("POST");
				StopMatchmakingRequest->SetHeader("Content-Type", "application/json");
				StopMatchmakingRequest->SetHeader("Authorization", AccessToken);
				StopMatchmakingRequest->SetContentAsString(RequestBody);
				StopMatchmakingRequest->ProcessRequest();
			}
			else {
				UTextBlock* ButtonTextBlock = (UTextBlock*)JoinButton->GetChildAt(0);
				ButtonTextBlock->SetText(FText::FromString("READY"));
				// JoinEventTextBlock->SetText(FText::FromString("READY"));
				JoinButton->SetIsEnabled(true);
			}
		}
		else {
			UTextBlock* ButtonTextBlock = (UTextBlock*)JoinButton->GetChildAt(0);
			ButtonTextBlock->SetText(FText::FromString("READY"));
			// JoinEventTextBlock->SetText(FText::FromString("READY"));
			JoinButton->SetIsEnabled(true);
		}
	}
	else {
		// Player Latency�� �����ϴ� �κ������� �ʿ����?

		if (AccessToken.Len() > 0 && false == StartMath) {
			StartMath = true;

			TSharedRef<FJsonObject> LatencyMapObj = MakeShareable(new FJsonObject);
			LatencyMapObj->SetNumberField(PokeHunterGameInstance->RegionCode, AveragePlayerLatency);

			TSharedPtr<FJsonObject> RequestObj = MakeShareable(new FJsonObject);
			RequestObj->SetObjectField("latencyMap", LatencyMapObj);

			FString RequestBody;
			TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
			if (FJsonSerializer::Serialize(RequestObj.ToSharedRef(), Writer)) {
				TSharedRef<IHttpRequest> StartJoinRequest = gameinstance->HttpModule->CreateRequest();
				StartJoinRequest->OnProcessRequestComplete().BindUObject(this, &UPartyInfoUI::OnStartMatchmakingResponseReceived);
				StartJoinRequest->SetURL(PokeHunterGameInstance->ApiUrl + "/startmatchmaking");
				StartJoinRequest->SetVerb("POST");
				StartJoinRequest->SetHeader("Content-Type", "application/json");
				StartJoinRequest->SetHeader("Authorization", AccessToken);
				StartJoinRequest->SetContentAsString(RequestBody);
				StartJoinRequest->ProcessRequest();
			}
			else {
				JoinButton->SetIsEnabled(true);
			}
		}
		else {
			JoinButton->SetIsEnabled(true);
		}
	}
	*/
}

void UPartyInfoUI::OnStartMatchmakingResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful) {
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(Reader, JsonObject)) {
			if (JsonObject->HasField("ticketId")) {
				FString MatchmakingTicketId = JsonObject->GetStringField("ticketId");

				UGameInstance* GameInstance = GetGameInstance();
				if (GameInstance != nullptr) {
					UBaseInstance* baseinstance = Cast<UBaseInstance>(GameInstance);
					if (baseinstance != nullptr) {
						baseinstance->JoinTicketId = MatchmakingTicketId;
						GetWorld()->GetTimerManager().SetTimer(PollMatchmakingHandle, this, &UPartyInfoUI::PollMatchmaking, 1.0f, true, 0.f);
						SearchingForGame = true;
					}
				}
			}
		}
	}
}

void UPartyInfoUI::PollMatchmaking()
{
	FString AccessToken;
	FString MatchmakingTicketId;

	UGameInstance* GameInstance = GetGameInstance();
	UBaseInstance* PokeHunterGameInstance = Cast<UBaseInstance>(GameInstance);
	if (GameInstance != nullptr) {
		if (PokeHunterGameInstance != nullptr) {
			AccessToken = PokeHunterGameInstance->IdToken;
			MatchmakingTicketId = PokeHunterGameInstance->JoinTicketId;
		}
	}

	if (AccessToken.Len() > 0 && MatchmakingTicketId.Len() > 0 && SearchingForGame) {
		TSharedPtr<FJsonObject> RequestObj = MakeShareable(new FJsonObject);
		RequestObj->SetStringField("ticketId", MatchmakingTicketId);

		FString RequestBody;
		TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
		if (FJsonSerializer::Serialize(RequestObj.ToSharedRef(), Writer)) {
			TSharedRef<IHttpRequest> PollMatchmakingRequest = gameinstance->HttpModule->CreateRequest();
			PollMatchmakingRequest->OnProcessRequestComplete().BindUObject(this, &UPartyInfoUI::OnPollMatchmakingResponseReceived);
			PollMatchmakingRequest->SetURL(PokeHunterGameInstance->ApiUrl + "/pollmatchmaking");
			PollMatchmakingRequest->SetVerb("POST");
			PollMatchmakingRequest->SetHeader("Content-Type", "application/json");
			PollMatchmakingRequest->SetHeader("Authorization", AccessToken);
			PollMatchmakingRequest->SetContentAsString(RequestBody);
			PollMatchmakingRequest->ProcessRequest();
		}
	}
}

void UPartyInfoUI::OnGetPlayerDataResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
	if (bWasSuccessful) {
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(Reader, JsonObject)) {
			if (JsonObject->HasField("playerData")) {

				// UMG�� ǥ�õ� �����͵��� ���������� �ʿ�����Ƿ� ����.
				// TSharedPtr<FJsonObject> PlayerData = JsonObject->GetObjectField("playerData");
				// WebBrowser->SetVisibility(ESlateVisibility::Hidden);

				TSharedPtr<FJsonObject> PlayerData = JsonObject->GetObjectField("playerData");
				TSharedPtr<FJsonObject> WinsObject = PlayerData->GetObjectField("Wins");
				TSharedPtr<FJsonObject> LossesObject = PlayerData->GetObjectField("Losses");

				TSharedPtr<FJsonObject> PartyNumberObject = PlayerData->GetObjectField("PartyNumber");
				FString PartyNumber = PartyNumberObject->GetObjectField("Type")->GetStringField("S");

				UE_LOG(LogTemp, Warning, TEXT("Get Player Data!"));
			}
		}
	}
}

void UPartyInfoUI::OnStopMatchmakingResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	UTextBlock* ButtonTextBlock = (UTextBlock*)JoinButton->GetChildAt(0);
	ButtonTextBlock->SetText(FText::FromString("Stop, READY"));
	// ButtonTextBlock->SetText(FText::FromString("READY"));
	// JoinEventTextBlock->SetText(FText::FromString("Stop READY")); // ->????

	JoinButton->SetIsEnabled(true);
}

void UPartyInfoUI::OnPollMatchmakingResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful && SearchingForGame) {
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(Reader, JsonObject)) {
			if (JsonObject->HasField("ticket")) {
				TSharedPtr<FJsonObject> Ticket = JsonObject->GetObjectField("ticket");
				FString TicketType = Ticket->GetObjectField("Type")->GetStringField("S");

				if (TicketType.Len() > 0) {
					// SearchingForGame = false;

					UGameInstance* GameInstance = GetGameInstance();
					if (GameInstance != nullptr) {
						UBaseInstance* PokeHunterGameInstance = Cast<UBaseInstance>(GameInstance);
						if (PokeHunterGameInstance != nullptr) {
							// PokeHunterGameInstance->JoinTicketId = "";
						}

						if (TicketType.Equals("MatchmakingSucceeded")) {
							GetWorld()->GetTimerManager().ClearTimer(PollMatchmakingHandle);
							GetWorld()->GetTimerManager().ClearTimer(SetAveragePlayerLatencyHandle);

							// Set Player input Mode;
							FInputModeGameOnly gameModeOnly;
							PokeHunterGameInstance->cur_playerController->bShowMouseCursor = false;
							PokeHunterGameInstance->cur_playerController->SetInputMode(gameModeOnly);

							// MatchmakingSucceeded �� ��쿡�� ����
							GetWorld()->GetTimerManager().ClearTimer(PollMatchmakingHandle);
							SearchingForGame = false;

							TSharedPtr<FJsonObject> GameSessionInfo = Ticket->GetObjectField("GameSessionInfo")->GetObjectField("M");
							FString IpAddress = GameSessionInfo->GetObjectField("IpAddress")->GetStringField("S");
							FString Port = GameSessionInfo->GetObjectField("Port")->GetStringField("N");

							TArray<TSharedPtr<FJsonValue>> Players = Ticket->GetObjectField("Players")->GetArrayField("L");
							TSharedPtr<FJsonObject> Player = Players[0]->AsObject()->GetObjectField("M");
							FString PlayerSessionId = Player->GetObjectField("PlayerSessionId")->GetStringField("S");
							FString PlayerId = Player->GetObjectField("PlayerId")->GetStringField("S");

							gameinstance->GameLiftLevelName = IpAddress + ":" + Port;
							const FString& Options = "?PlayerSessionId=" + PlayerSessionId + "?PlayerId=" + PlayerId;
							UE_LOG(LogTemp, Warning, TEXT("options: %s"), *Options);

							//==========================================================================================================
							/*
							UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
								TSubclassOf <UUserWidget> LoadingUIClass;
							UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
								class UUserWidget* LoadingUI;

							static ConstructorHelpers::FClassFinder<UUserWidget>TempLoadingClass(TEXT("/Game/UI/LoadingScreen/UI_LoadingScreen"));
							if (TempLoadingClass.Succeeded())
							{
								LoadingUIClass = TempLoadingClass.Class;
							}
							LoadingUI = CreateWidget(GetWorld(), LoadingUIClass, TEXT("LogoutUI"));
							LoadingUI->AddToViewport();

							FTimerHandle DelayTimerHandle;
							float DelayTime = 5.0f;

							GetWorld()->GetTimerManager().SetTimer(DelayTimerHandle, this, &UPartyInfoUI::LoadingScreenCall, DelayTime, false);
							while (GetWorld()->GetTimerManager().IsTimerActive(DelayTimerHandle) && !GetWorld()->IsPendingKill())
							{
								// ���� ������ Tick �̺�Ʈ�� ������ �����Ͽ� Delay Ÿ�̸Ӹ� �����ŵ�ϴ�.
								GetWorld()->Tick(LEVELTICK_All, 0.0f);
							}
							*/
							//==========================================================================================================

							UE_LOG(LogTemp, Warning, TEXT("[AWS] Server Addr : %s:%s"), *gameinstance->GameLiftLevelName, *Port);
							UGameplayStatics::OpenLevel(GetWorld(), FName(gameinstance->GameLiftLevelName), false, Options);
						}
						else {
							// AWS GameLift Dedicated server ���ӿ� �����Ͽ���.
							// UTextBlock* ButtonTextBlock = (UTextBlock*)JoinButton->GetChildAt(0);
							// ButtonTextBlock->SetText(FText::FromString("FreREADY"));
							// JoinEventTextBlock->SetText(FText::FromString(TicketType + ". Please try again"));
						}
					}
				}
			}
		}
	}
}
