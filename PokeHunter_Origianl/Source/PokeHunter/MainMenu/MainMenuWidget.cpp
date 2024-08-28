// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MainMenuWidget.h"
#include "TextReaderComponent.h"
#include "WebBrowser.h"
#include "WebBrowserModule.h"
#include "IWebBrowserSingleton.h"
#include "IWebBrowserCookieManager.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "PokeHunter/Base/BaseInstance.h"

#include "Kismet/GameplayStatics.h"

UMainMenuWidget::UMainMenuWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	UTextReaderComponent* TextReader = CreateDefaultSubobject<UTextReaderComponent>(TEXT("TextReaderComp"));

	LoginUrl = TextReader->ReadFile("Urls/LoginUrl.txt");
	ApiUrl = TextReader->ReadFile("Urls/ApiUrl.txt");
	CallbackUrl = TextReader->ReadFile("Urls/CallbackUrl.txt");

	gameinstance = Cast<UBaseInstance>(UGameplayStatics::GetGameInstance((GetWorld())));
}

void UMainMenuWidget::NativeConstruct() {
	Super::NativeConstruct();
	SetIsFocusable(true);

	WebBrowser = (UWebBrowser*)GetWidgetFromName(TEXT("WebBrowser_Login"));

	IWebBrowserSingleton* WebBrowserSingleton = IWebBrowserModule::Get().GetSingleton();

	if (WebBrowserSingleton != nullptr) {
		TOptional<FString> DefaultContext;
		TSharedPtr<IWebBrowserCookieManager> CookieManager = WebBrowserSingleton->GetCookieManager(DefaultContext);
		if (CookieManager.IsValid()) {
			CookieManager->DeleteCookies();
		}
	}

	WebBrowser->LoadURL(LoginUrl);

	FScriptDelegate LoginDelegate;
	LoginDelegate.BindUFunction(this, "HandleLoginUrlChange");
	WebBrowser->OnUrlChanged.Add(LoginDelegate);
}

void UMainMenuWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UMainMenuWidget::HandleLoginUrlChange()
{
	FString BrowserUrl = WebBrowser->GetUrl();
	FString Url;
	FString QueryParameters;

	// UE_LOG(LogTemp, Warning, TEXT("BrowserUrl: %s"), *BrowserUrl);

	if (BrowserUrl.Split("?", &Url, &QueryParameters)) {
		if (Url.Equals(CallbackUrl)) {
			FString ParameterName;
			FString ParameterValue;

			if (QueryParameters.Split("=", &ParameterName, &ParameterValue)) {
				if (ParameterName.Equals("code")) {
					// ParameterValue = ParameterValue.Replace(*FString("#"), *FString(""));

					TSharedPtr<FJsonObject> RequestObj = MakeShareable(new FJsonObject);
					RequestObj->SetStringField(ParameterName, ParameterValue);

					FString RequestBody;
					TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);

					UE_LOG(LogTemp, Warning, TEXT("ParameterValue: %s"), *ParameterValue);

					if (FJsonSerializer::Serialize(RequestObj.ToSharedRef(), Writer)) {
						TSharedRef<IHttpRequest> ExchangeCodeForTokensRequest = gameinstance->HttpModule->CreateRequest();
						ExchangeCodeForTokensRequest->OnProcessRequestComplete().BindUObject(this, &UMainMenuWidget::OnExchangeCodeForTokensResponseReceived);
						ExchangeCodeForTokensRequest->SetURL(ApiUrl + "/exchangecodefortokens");
						ExchangeCodeForTokensRequest->SetVerb("POST");
						ExchangeCodeForTokensRequest->SetHeader("Content-Type", "application/json");
						ExchangeCodeForTokensRequest->SetContentAsString(RequestBody);
						ExchangeCodeForTokensRequest->ProcessRequest();
					}
				}
			}
		}
	}
}

void UMainMenuWidget::OnExchangeCodeForTokensResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful) {
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(Reader, JsonObject)) {
			if (JsonObject->HasField("access_token") && JsonObject->HasField("id_token") && JsonObject->HasField("refresh_token")) {
				UGameInstance* GameInstance = GetGameInstance();
				if (GameInstance != nullptr) {
					UBaseInstance* ServerIntance = Cast<UBaseInstance>(GameInstance);
					if (ServerIntance != nullptr) {
						FString AccessToken = JsonObject->GetStringField("access_token");
						FString IdToken = JsonObject->GetStringField("id_token");
						FString RefreshToken = JsonObject->GetStringField("refresh_token");
						
						ServerIntance->SetCognitoTokens(AccessToken, IdToken, RefreshToken);

						ShowLoadingWidget();

						FString LevelName = GetWorld()->GetName();
						if ("Title" == LevelName) {
							FString levelName = L"/Game/Map/Lobby/MyHome";
							UGameplayStatics::OpenLevel(GetWorld(), *levelName);
						}
						NativeDestruct();
					}
				}
			}
		}
	}
}
