// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseInstance.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "Kismet/GameplayStatics.h"
#include "PokeHunter/MainMenu/TextReaderComponent.h"

UBaseInstance::UBaseInstance()
{
	UTextReaderComponent* TextReader = CreateDefaultSubobject<UTextReaderComponent>(TEXT("TextReaderComp"));
	ApiUrl = TextReader->ReadFile("Urls/ApiUrl.txt");
	RegionCode = TextReader->ReadFile("Urls/RegionCode.txt");
	HttpModule = &FHttpModule::Get();
}

void UBaseInstance::Init()
{
	Super::Init();
}

bool UBaseInstance::ConnectToServer(FString server_addr)
{
	FIPv4Address::Parse(server_addr, ip);
	addr->SetIp(ip.Value);
	addr->SetPort(PORT_NUM);

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Trying to connect.")));

	if (true == gSocket->Connect(*addr)) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("[Success] connect to server")));
		return true;
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("[Fail] connect to server")));
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
		else {
			UE_LOG(LogTemp, Warning, TEXT("Token Send Success"));
		}
	}

	SC_LOGIN_SUCCESS_PACK ok_pack;
	retVal = gSocket->Recv(reinterpret_cast<uint8*>(&ok_pack), sizeof(SC_LOGIN_SUCCESS_PACK), bSize);

	if (false == retVal) {
		UE_LOG(LogTemp, Warning, TEXT("Login Fail!"));
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Login Succcccccccccccccesssssssssssssss!!!!!!"));

		SC_LOGIN_INFO_PACK info_pack;
		retVal = gSocket->Recv(reinterpret_cast<uint8*>(&info_pack), sizeof(SC_LOGIN_INFO_PACK), bSize);
		if (false == retVal) {

			UE_LOG(LogTemp, Warning, TEXT("Fail get player infomation!"));
		}
		else {
			// 안전하게 uint8 -> EPartnerType 로 변환
			TCHAR MBTWBuffer[128];
			memset(MBTWBuffer, NULL, 128);
			MultiByteToWideChar(CP_ACP, 0, (LPCSTR)info_pack._pet_num, -1, MBTWBuffer, strlen(info_pack._pet_num));

			uint8 partner_number = atoi(TCHAR_TO_ANSI(MBTWBuffer));

			if (partner_number <= (sizeof(EPartnerType) / sizeof(uint8))) {
				myPartner = static_cast<EPartnerType>(partner_number);
				UE_LOG(LogTemp, Warning, TEXT("Successssssss get player infomation! %d"), partner_number);
			}
			else {
				UE_LOG(LogTemp, Warning, TEXT("Fail Get Partner number: %d"), partner_number);
			}
		}
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
		CS_LOGOUT_PACK logout_pack;
		logout_pack.size = sizeof(CS_LOGOUT_PACK);
		logout_pack.type = CS_LOGOUT;
		gSocket->Send(reinterpret_cast<const uint8*>(&logout_pack), logout_pack.size, bSize);
	}

	Super::Shutdown();
}

void UBaseInstance::SetCognitoTokens(FString NewAccessToken, FString NewIdToken, FString NewRefreshToken)
{
	FString LevelName = GetWorld()->GetName();
	if ("Title" == LevelName) {
		FString levelName = L"/Game/Map/Lobby/MyHome";
		UGameplayStatics::OpenLevel(GetWorld(), *levelName);
	}

	NewAccessToken.ReplaceInline(TEXT("\n"), TEXT(""));
	AccessToken = NewAccessToken;

	NewIdToken.ReplaceInline(TEXT("\n"), TEXT(""));
	IdToken = NewIdToken;
	
	NewRefreshToken.ReplaceInline(TEXT("\n"), TEXT(""));
	RefreshToken = NewRefreshToken;

	UE_LOG(LogTemp, Warning, TEXT("[X]access token: %s"), *AccessToken);
	UE_LOG(LogTemp, Warning, TEXT("[O]IdToken token: %s"), *IdToken);
	UE_LOG(LogTemp, Warning, TEXT("refresh token: %s"), *RefreshToken);

	// World Timer에 등록하기
	GetWorld()->GetTimerManager().SetTimer(RetrieveNewTokensHandle, this, &UBaseInstance::RetrieveNewTokens, 1.0f, false, 3300.0f);
}

bool UBaseInstance::returnMyHome()
{
	// GetWorld()->GetTimerManager().ClearTimer(RetrieveNewTokensHandle);
	// GetWorld()->GetTimerManager().ClearTimer(GetResponseTimeHandle);

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

				return true;
			}

		}
		
	}

	return false;
}

void UBaseInstance::RetrieveNewTokens()
{
	if (IdToken.Len() > 0 && RefreshToken.Len() > 0) {
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
			RetrieveNewTokensRequest->SetHeader("Authorization", IdToken);
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
			if (JsonObject->HasField("access_token") && JsonObject->HasField("id_token") && JsonObject->HasField("refresh_token")) {
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
	//UE_LOG(LogTemp, Warning, TEXT("response time in milliseconds: %s"), *FString::SanitizeFloat(ResponseTime));

	PlayerLatencies.AddTail(ResponseTime);
}
