// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseInstance.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "Kismet/GameplayStatics.h"
#include "PokeHunter/MainMenu/TextReaderComponent.h"
#include <Net/UnrealNetwork.h>

UBaseInstance::UBaseInstance()
{
	TextReader = CreateDefaultSubobject<UTextReaderComponent>(TEXT("TextReaderComp"));
	ApiUrl = TextReader->ReadFile("Urls/ApiUrl.txt");
	RegionCode = TextReader->ReadFile("Urls/RegionCode.txt");
	HttpModule = &FHttpModule::Get();

	mySkin = 1;
	PartnerNumber = -1;
	myPartner = EPartnerType::NonePartner;
	
	InfoArray.Reserve(24);
	StorageInfoArray.Reserve(24);

	gSocket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(TEXT("Stream"), TEXT("Client Socket"));
	addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();

	PlayerPetName = { EPartnerType::WolfPartner, EPartnerType::WolfPartner, EPartnerType::WolfPartner, EPartnerType::WolfPartner };

	for (int i = 0; i < 4; ++i)
	{
		PartnerSkillArray.AddDefaulted();
	}

	endGame = false;
}

void UBaseInstance::Init()
{
	Super::Init();
}

bool UBaseInstance::ConnectToServer()
{
	if (ESocketConnectionState::SCS_Connected == gSocket->GetConnectionState()) return false;	// 다시 연결할 필요 없으므로 false
	if (nullptr == TextReader) return false;
	FString server_addr = TextReader->ReadFile("Urls/IOCPServerAddr.txt");

	FIPv4Address::Parse(server_addr, ip);
	addr->SetIp(ip.Value);
	addr->SetPort(PORT_NUM);

	if (true == gSocket->Connect(*addr)) {
		return true;
	}
	else {
		ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
		ESocketErrors ErrorCode = SocketSubsystem->GetLastErrorCode();
		FString ErroCodeAsString = FString(SocketSubsystem->GetSocketError(ErrorCode));
		GEngine->AddOnScreenDebugMessage(-1, 60.0f, FColor::Red, FString::Printf(TEXT("[Fail] connect to server\nRe: %s"), *ErroCodeAsString));

		return false;
	}
}

bool UBaseInstance::SendAccessToken()
{
	if (NULL == gSocket) return false;
	if (ESocketConnectionState::SCS_NotConnected == gSocket->GetConnectionState()) return false;
	if (ESocketConnectionState::SCS_ConnectionError == gSocket->GetConnectionState()) return false;

	bool retVal = false;
	int32 bSize = 0;
	CS_LOGIN_PACK token_pack;
	token_pack.size = (char)sizeof(CS_LOGIN_PACK);
	token_pack.type = CS_LOGIN;

	short last_index = AccessToken.Len();

	for (int i = 0; i < 2000; i += 50) {
		if (last_index < i) {
			strncpy_s(token_pack.Token, "theEnd", strlen("theEnd"));
			retVal = gSocket->Send(reinterpret_cast<const uint8*>(&token_pack), token_pack.size, bSize);
			break;
		}

		if (50 > last_index - i) {
			token_pack.Token_size = AccessToken.Mid(i, last_index).Len();
			WideCharToMultiByte(CP_ACP, 0, *AccessToken.Mid(i, last_index), AccessToken.Mid(i, last_index).Len(), token_pack.Token, sizeof(token_pack.Token), NULL, NULL);
		}
		else {
			token_pack.Token_size = 50;
			WideCharToMultiByte(CP_ACP, 0, *AccessToken.Mid(i, i + 50), AccessToken.Mid(i, i + 50).Len(), token_pack.Token, sizeof(token_pack.Token), NULL, NULL);
		}
		
		retVal = gSocket->Send(reinterpret_cast<const uint8*>(&token_pack), token_pack.size, bSize);
		if (false == retVal) {
			UE_LOG(LogTemp, Warning, TEXT("Token Send Fail"));
			int32 ErrorCode = GetLastError();
		}
	}

	SC_LOGIN_SUCCESS_PACK ok_pack;
	retVal = gSocket->Recv(reinterpret_cast<uint8*>(&ok_pack), sizeof(SC_LOGIN_SUCCESS_PACK), bSize);

	if (false == retVal) {
		UE_LOG(LogTemp, Warning, TEXT("Login Fail!"));
	}
	else {
		SC_LOGIN_INFO_PACK info_pack;
		retVal = gSocket->Recv(reinterpret_cast<uint8*>(&info_pack), sizeof(SC_LOGIN_INFO_PACK), bSize);
		if (false == retVal) {
			UE_LOG(LogTemp, Warning, TEXT("Fail get player infomation!"));
			return false;
		}
		else {
			TCHAR MBTWBuffer[128];
			memset(MBTWBuffer, NULL, 128);
			MultiByteToWideChar(CP_ACP, 0, (LPCSTR)info_pack._pet_num, -1, MBTWBuffer, strlen(info_pack._pet_num));

			uint8 partner_number = atoi(TCHAR_TO_ANSI(MBTWBuffer));

			if (partner_number <= (sizeof(EPartnerType) / sizeof(uint8))) {
				myPartner = static_cast<EPartnerType>(partner_number);
				mySkin = static_cast<int>(info_pack._player_skin - '0');
			}

			memset(MBTWBuffer, NULL, 128);
			MultiByteToWideChar(CP_ACP, 0, (LPCSTR)info_pack._q_skill, -1, MBTWBuffer, strlen(info_pack._q_skill));
			for (int i = 0; i < 4; ++i) {
				PartnerSkillArray[i] = (ESkillID)((uint8)(MBTWBuffer[i] - '0'));
			}


		}
	}
	return true;
}

bool UBaseInstance::LeaveParty()
{
	bool retVal = false;
	int32 bSize;

	CS_PARTY_LEAVE_PACK leave_pack;
	leave_pack.size = sizeof(CS_PARTY_LEAVE_PACK);
	leave_pack.type = CS_PARTY_LEAVE;
	retVal = gSocket->Send(reinterpret_cast<const uint8*>(&leave_pack), leave_pack.size, bSize);

	SC_PARTY_LEAVE_SUCCESS_PACK ok_pack;
	ok_pack.size = sizeof(SC_PARTY_LEAVE_SUCCESS_PACK);
	ok_pack.type = SC_PARTY_LEAVE_SUCCESS;

	retVal = gSocket->Recv(reinterpret_cast<uint8*>(&ok_pack), sizeof(SC_PARTY_LEAVE_SUCCESS_PACK), bSize);
	if (false == retVal) {
		return false;
	}

	int32 leave_cnt = PlayerName.Find(FName(ok_pack._mem));

	if (-1 != leave_cnt) {
		PlayerName.RemoveAt(leave_cnt);
		PlayerPetName.RemoveAt(leave_cnt);
		PartyMemberState.RemoveAt(leave_cnt);
	}
	else {
		return false;
	}
	return true;
}

// 클라이언트 종료시에 호출되는 함수
void UBaseInstance::Shutdown()
{
	GetWorld()->GetTimerManager().ClearTimer(RetrieveNewTokensHandle);
	GetWorld()->GetTimerManager().ClearTimer(GetResponseTimeHandle);

	if (AccessToken.Len() > 0) {
		if (JoinTicketId.Len() > 0) {
			TSharedPtr<FJsonObject> RequestObj = MakeShareable(new FJsonObject);
			RequestObj->SetStringField("ticketId", JoinTicketId);

			FString RequestBody;
			TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
			if (FJsonSerializer::Serialize(RequestObj.ToSharedRef(), Writer)) {
				TSharedRef<IHttpRequest> StopMatchmakingRequest = HttpModule->CreateRequest();
				StopMatchmakingRequest->SetURL(ApiUrl + "/stopmatchmaking");
				StopMatchmakingRequest->SetVerb("POST");
				StopMatchmakingRequest->SetHeader("Content-Type", "application/json");
				StopMatchmakingRequest->SetHeader("Authorization", AccessToken);
				StopMatchmakingRequest->SetContentAsString(RequestBody);
				StopMatchmakingRequest->ProcessRequest();
			}
		}
		TSharedRef<IHttpRequest> InvalidateTokensRequest = HttpModule->CreateRequest();
		InvalidateTokensRequest->SetURL(ApiUrl + "/invalidatetokens");
		InvalidateTokensRequest->SetVerb("GET");
		//InvalidateTokensRequest->SetHeader("Content-Type", "application/json");
		InvalidateTokensRequest->SetHeader("Authorization", AccessToken);
		InvalidateTokensRequest->ProcessRequest();
	}

	if ((NULL != gSocket) && (ESocketConnectionState::SCS_NotConnected != gSocket->GetConnectionState()) && (ESocketConnectionState::SCS_ConnectionError != gSocket->GetConnectionState()))
	{
		int32 bSize = 0;
		CS_SAVE_INVENTORY_PACK item_pack;
		item_pack.size = sizeof(CS_SAVE_INVENTORY_PACK);
		item_pack.type = CS_SAVE_INVENTORY;
		for (FItemCnter& it : InfoArray) {
			if (FName("None") != it.ItemID) {
				ZeroMemory(item_pack._name, sizeof(item_pack._name));
				strcpy(item_pack._name, TCHAR_TO_ANSI(*it.ItemID.ToString()));
				item_pack._cnt = it.cnt;
				gSocket->Send(reinterpret_cast<const uint8*>(&item_pack), item_pack.size, bSize);
			}
		}

		CS_LOGOUT_PACK logout_pack;
		logout_pack.size = sizeof(CS_LOGOUT_PACK);
		logout_pack.type = CS_LOGOUT;
		bool retVal = gSocket->Send(reinterpret_cast<const uint8*>(&logout_pack), logout_pack.size, bSize);
	}

	Super::Shutdown();
}

bool UBaseInstance::LogoutGame()
{
	GetWorld()->GetTimerManager().ClearTimer(RetrieveNewTokensHandle);
	GetWorld()->GetTimerManager().ClearTimer(GetResponseTimeHandle);

	if (AccessToken.Len() > 0) {
		if (JoinTicketId.Len() > 0) {
			TSharedPtr<FJsonObject> RequestObj = MakeShareable(new FJsonObject);
			RequestObj->SetStringField("ticketId", JoinTicketId);

			FString RequestBody;
			TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
			if (FJsonSerializer::Serialize(RequestObj.ToSharedRef(), Writer)) {
				TSharedRef<IHttpRequest> StopMatchmakingRequest = HttpModule->CreateRequest();
				StopMatchmakingRequest->SetURL(ApiUrl + "/stopmatchmaking");
				StopMatchmakingRequest->SetVerb("POST");
				StopMatchmakingRequest->SetHeader("Content-Type", "application/json");
				StopMatchmakingRequest->SetHeader("Authorization", AccessToken);
				StopMatchmakingRequest->SetContentAsString(RequestBody);
				StopMatchmakingRequest->ProcessRequest();
			}
			else {
				// UE_LOG(LogTemp, Warning, TEXT("stopmatchmaking Lambda Function Error! or JoinTicketId is Empty"));
			}
		}
		else {
			// UE_LOG(LogTemp, Warning, TEXT("JoinTicketId is Emtpy!"));
		}

		TSharedRef<IHttpRequest> InvalidateTokensRequest = HttpModule->CreateRequest();
		InvalidateTokensRequest->SetURL(ApiUrl + "/invalidatetokens");
		InvalidateTokensRequest->SetVerb("GET");
		InvalidateTokensRequest->SetHeader("Content-Type", "application/json");
		InvalidateTokensRequest->SetHeader("Authorization", AccessToken);
		InvalidateTokensRequest->ProcessRequest();
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("AccessToken Error!"));
		return false;
	}

	if ((ESocketConnectionState::SCS_NotConnected != gSocket->GetConnectionState()) && (ESocketConnectionState::SCS_ConnectionError != gSocket->GetConnectionState()))
	{
		int32 bSize = 0;
		bool retVal = true;
		CS_SAVE_INVENTORY_PACK item_pack;
		item_pack.size = sizeof(CS_SAVE_INVENTORY_PACK);
		item_pack.type = CS_SAVE_INVENTORY;
		for (FItemCnter& it : InfoArray) {
			if (FName("None") != it.ItemID) {
				ZeroMemory(item_pack._name, sizeof(item_pack._name));
				strcpy(item_pack._name, TCHAR_TO_ANSI(*it.ItemID.ToString()));
				item_pack._cnt = it.cnt;
				retVal = gSocket->Send(reinterpret_cast<const uint8*>(&item_pack), item_pack.size, bSize);
				if (false == retVal) return false;
			}
		}
		strcpy(item_pack._name, "theEnd");
		retVal = gSocket->Send(reinterpret_cast<const uint8*>(&item_pack), item_pack.size, bSize);
		if (false == retVal) return false;

		CS_LOGOUT_PACK logout_pack;
		logout_pack.size = sizeof(CS_LOGOUT_PACK);
		logout_pack.type = CS_LOGOUT;
		retVal = gSocket->Send(reinterpret_cast<const uint8*>(&logout_pack), logout_pack.size, bSize);
		if (false == retVal) return false;

		bSize = 0;
		SC_LOGOUT_RESULT_PACK out_client;
		retVal = gSocket->Recv(reinterpret_cast<uint8*>(&out_client), sizeof(SC_LOGOUT_RESULT_PACK), bSize);
		if (false == retVal) return false;

		TCHAR MBTWBuffer[128];
		memset(MBTWBuffer, NULL, 128);
		MultiByteToWideChar(CP_ACP, 0, (LPCSTR)out_client._result, -1, MBTWBuffer, strlen(out_client._result));

		uint8 partner_number = atoi(TCHAR_TO_ANSI(MBTWBuffer));
		if (0 != partner_number) {
			gSocket->Close();
			gSocket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(TEXT("Stream"), TEXT("Client Socket"));
			addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
			ip = NULL;

			// Reset Game instance data
			resetGameInstance();

			return true;
		}
		else {
			return false;
		}
	}
	else 
	{
		UE_LOG(LogTemp, Warning, TEXT("FSocket Error!"));
		return false;
	}
}

void UBaseInstance::SetCognitoTokens(FString NewAccessToken, FString NewIdToken, FString NewRefreshToken)
{
	AccessToken = NewAccessToken;
	IdToken = NewIdToken;
	RefreshToken = NewRefreshToken;

	// World Timer에 등록하기
	GetWorld()->GetTimerManager().SetTimer(RetrieveNewTokensHandle, this, &UBaseInstance::RetrieveNewTokens, 1.0f, false, 30.0f);
}

void UBaseInstance::RetrieveNewTokens()
{
	if (AccessToken.Len() > 0 && RefreshToken.Len() > 0) {
		TSharedPtr<FJsonObject> RequestObj = MakeShareable(new FJsonObject);
		RequestObj->SetStringField("refreshToken", RefreshToken);

		FString RequestBody;
		TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);

		if (FJsonSerializer::Serialize(RequestObj.ToSharedRef(), Writer)) {
			TSharedRef<IHttpRequest> RetrieveNewTokensRequest = HttpModule->CreateRequest();
			RetrieveNewTokensRequest->OnProcessRequestComplete().BindUObject(this, &UBaseInstance::OnRetrieveNewTokensResponseReceived);
			RetrieveNewTokensRequest->SetURL(ApiUrl + "/retrievenewtokens");
			RetrieveNewTokensRequest->SetVerb("POST");
			RetrieveNewTokensRequest->SetHeader("Content-Type", "application/json");
			RetrieveNewTokensRequest->SetHeader("Authorization", AccessToken);
			RetrieveNewTokensRequest->SetContentAsString(RequestBody);
			RetrieveNewTokensRequest->ProcessRequest();
		}
		else {
			GetWorld()->GetTimerManager().SetTimer(RetrieveNewTokensHandle, this, &UBaseInstance::RetrieveNewTokens, 1.0f, false, 30.0f);
		}
	}
}

void UBaseInstance::OnRetrieveNewTokensResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful) {
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(Reader, JsonObject)) {
			if (JsonObject->HasField("accessToken") && JsonObject->HasField("idToken")) {
				SetCognitoTokens(JsonObject->GetStringField("accessToken"), JsonObject->GetStringField("idToken"), RefreshToken);
			}
		}
		else {
			GetWorld()->GetTimerManager().SetTimer(RetrieveNewTokensHandle, this, &UBaseInstance::RetrieveNewTokens, 1.0f, false, 30.0f);
		}
	}
	else {
		GetWorld()->GetTimerManager().SetTimer(RetrieveNewTokensHandle, this, &UBaseInstance::RetrieveNewTokens, 1.0f, false, 30.0f);
	}
}

void UBaseInstance::OnGetResponseTimeResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (PlayerLatencies.Num() >= 4) {
		PlayerLatencies.RemoveNode(PlayerLatencies.GetHead());
	}

	float ResponseTime = Request->GetElapsedTime() * 1000;

	PlayerLatencies.AddTail(ResponseTime);
}

void UBaseInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// RepNotify 설정
	DOREPLIFETIME(UBaseInstance, mySkin);
}

double UBaseInstance::getElapseTime()
{
	if (endTime < 1.0f) {
		endTime = GetWorld()->GetTimeSeconds();
	}

	return (endTime);
}

void UBaseInstance::resetGameInstance()
{
	if (false == Player_Name.IsEmpty()) Player_Name.Empty();
	if (false == Player_Skin.IsEmpty()) Player_Skin.Empty();
	if (false == Pet_Number.IsEmpty()) Pet_Number.Empty();
	if (false == Quick_Item.IsEmpty()) Quick_Item.Empty();
	if (false == Quick_Skill.IsEmpty()) Quick_Skill.Empty();

	myPartner = EPartnerType::NonePartner;

	AccessToken = NULL;
	IdToken = NULL;
	RefreshToken = NULL;

	/*RetrieveNewTokensHandle.Invalidate();
	GetResponseTimeHandle.Invalidate();
	PartyInfoHandle.Invalidate();*/

	if (false == PlayerLatencies.IsEmpty()) PlayerLatencies.Empty();
	if (false == GameLiftLevelName.IsEmpty()) GameLiftLevelName.Empty();
	if (false == GameLiftLevelNameOptions.IsEmpty()) GameLiftLevelNameOptions.Empty();
	if (false == JoinTicketId.IsEmpty()) JoinTicketId.Empty();

	cur_playerController = nullptr;

	if (false == PartyListMap.IsEmpty()) PartyListMap.Empty();
	if (false == inStageParty.IsEmpty()) inStageParty.Empty();
	if (false == InfoArray.IsEmpty()) InfoArray.Empty();
	if (false == StorageInfoArray.IsEmpty()) StorageInfoArray.Empty();
	if (false == PartnerSkillArray.IsEmpty()) PartnerSkillArray.Empty();
	for (int i = 0; i < 4; ++i)
	{
		PartnerSkillArray.AddDefaulted();
	}

	InfoArray.Reserve(24);
	StorageInfoArray.Reserve(24);

	mySkin = 1;
	PartnerNumber = -1;
	endTime = 0.0f;
	endGame = false;

	myPartner = EPartnerType::NonePartner;

	gSocket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(TEXT("Stream"), TEXT("Client Socket"));
	addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();

	PlayerPetName = { EPartnerType::WolfPartner, EPartnerType::WolfPartner, EPartnerType::WolfPartner, EPartnerType::WolfPartner };
}
