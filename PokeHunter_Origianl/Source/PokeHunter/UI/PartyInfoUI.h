// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PokeHunter/Base/BaseInstance.h"
#include "PokeHunter/Network/PartyServerManager.h"

#include "CoreMinimal.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "PartyInfoUI.generated.h"

class UWebBrowser;
class UButton;
class UTextBlock;

/**
 * 
 */
UCLASS(BlueprintType)
class POKEHUNTER_API UPartyInfoUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPartyInfoUI(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:
	UBaseInstance* gameinstance = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Party info")
		int SelctPartyNumber = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Party info")
		TArray<FName> PlayerName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Party info")
		TArray<EPartnerType> PlayerPetName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Party info")
		TArray<PLAYER_STATE> PartyMemberState;

	UPROPERTY()
		FTimerHandle TH_Partyinfo;

	bool myReadyState = false;

	UFUNCTION(BlueprintCallable, Category = "Party info")
		void TickRecvPartyState();

	UFUNCTION(BlueprintCallable, Category = "Party info")
		void SendClientState();

	UFUNCTION(BlueprintCallable, Category = "Party info")
		bool SendEnterParty();

	UFUNCTION(BlueprintCallable, Category = "Party info")
		void TickSendPartyInfo();

	UFUNCTION(BlueprintCallable, Category = "Party info")
		bool RecvClientJoin();

	UFUNCTION(BlueprintCallable, Category = "Party info")
		bool LeaveParty();

	UFUNCTION(BlueprintCallable, Category = "Loading Screen")
		void LoadingScreenCall();

	UPROPERTY()
		bool WaitforOtherClient = true;
	
	UPROPERTY()
		bool StartMath = false;

	UPROPERTY()
		FTimerHandle TH_WaitOtherClient;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Party info")
		UButton* JoinButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Party info")
		UTextBlock* JoinEventTextBlock;

	UPROPERTY()
		bool SearchingForGame;
	
	UFUNCTION()
		void SetAveragePlayerLatency();

	UFUNCTION()
		void OnJoinButtonClicked();

	UFUNCTION(BlueprintImplementableEvent, Category = "Loading Widget")
		void ShowLoadingWidget();

	UPROPERTY()
		FTimerHandle PollMatchmakingHandle;

	UPROPERTY()
		FTimerHandle SetAveragePlayerLatencyHandle;

private:
	UPROPERTY()
		UWebBrowser* WebBrowser;

	UPROPERTY()
		float AveragePlayerLatency;

	UFUNCTION()
		void EnterStageMap();

	UFUNCTION()
		void PollMatchmaking();

	void OnGetPlayerDataResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void OnStartMatchmakingResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void OnStopMatchmakingResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void OnPollMatchmakingResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
};
