// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "PokeHunter/Base/BaseInstance.h"
#include "MainMenuWidget.generated.h"

class UWebBrowser;
class UButton;
class UTextBlock;

/**
 *
 */
UCLASS()
class POKEHUNTER_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UMainMenuWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:
	UBaseInstance* gameinstance = nullptr;

	UPROPERTY()
		FString LoginUrl;

	UPROPERTY()
		FString ApiUrl;

	UFUNCTION(BlueprintImplementableEvent, Category = "Loading Widget")
		void ShowLoadingWidget();

private:
	UPROPERTY()
		FString CallbackUrl;

	UPROPERTY()
		UWebBrowser* WebBrowser;

	UFUNCTION()
		void HandleLoginUrlChange();

	void OnExchangeCodeForTokensResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
};
