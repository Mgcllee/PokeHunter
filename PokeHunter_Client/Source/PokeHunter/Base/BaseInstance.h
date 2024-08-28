// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Runtime/Core/Public/Windows/AllowWindowsPlatformTypes.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "Runtime/Networking/Public/Networking.h"
#include "Runtime/Networking/Public/Interfaces/IPv4/IPv4Address.h"
#include "Runtime/Sockets/Public/Sockets.h"
#include "Runtime/Sockets/Public/SocketSubsystem.h"
#include "Runtime/Online/HTTP/Public/Http.h"

#include "protocol.h"

#include "Windows/WindowsPlatformMisc.h"
#include "Runtime/Core/Public/Windows/HideWindowsPlatformTypes.h"
#include "PokeHunter/Item/ItemData.h"
#include "PokeHunter/Base/SkillData.h"

#include "GameFramework/Controller.h"

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "BaseInstance.generated.h"

UENUM(BlueprintType)
enum class PLAYER_STATE : uint8
{
	ST_HOME = 0		UMETA(DisplayName = "State Home"),
	ST_NOTREADY		UMETA(DisplayName = "State Not Ready"),
	ST_READY		UMETA(DisplayName = "State Ready"),
	ST_STAGE		UMETA(DisplayName = "State Stage")
};

UENUM(BlueprintType)
enum class EPartnerType : uint8
{
	NonePartner = 0 UMETA(DisplayName = "None"),
	WolfPartner		UMETA(DisplayName = "Wolf"),
	GolemPartner	UMETA(DisplayName = "Golem")
};

class UTextReaderComponent;

/**
 * 
 */
USTRUCT(BlueprintType)
struct FU_SC_LOGIN_INFO_PACK {
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString _name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString _player_skin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString _pet_num;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString _q_item;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString _q_skill;
};

USTRUCT(BlueprintType)
struct FU_SC_ITEM_INFO_PACK {
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName _name {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 cnt {};
};

UCLASS()
class POKEHUNTER_API UBaseInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	// Title UI
	UTextReaderComponent* TextReader;

	// Hunter information
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Player_Name{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Player_Skin{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Pet_Number{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Quick_Item{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Quick_Skill{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EPartnerType myPartner{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FItemCnter> InfoArray{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FItemCnter> StorageInfoArray{};
	APlayerController* cur_playerController{};

	// Replicated
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, Category = "Rep")
		int32 mySkin{};

	// Network Socket
	FSocket* gSocket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(TEXT("Stream"), TEXT("Client Socket"));
	TSharedRef<FInternetAddr> addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	FIPv4Address ip{};

	// PartyInfoUI
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Party info")
		FName MyName{};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Party info")
		int PartnerNumber{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Party info")
		TArray<FName> PlayerName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Party info")
		TArray<EPartnerType> PlayerPetName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Party info")
		TArray<PLAYER_STATE> PartyMemberState;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Party info")
		TMap<FName, int> PartyListMap;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Party info")
		TArray<int32> inStageParty;

	// AWS Access information
	UPROPERTY(BlueprintReadOnly, Category = "AWS info")
		FString AccessToken;
	UPROPERTY(BlueprintReadOnly, Category = "AWS info")
		FString IdToken;
	UPROPERTY(BlueprintReadOnly, Category = "AWS info")
		FString RefreshToken;
	UPROPERTY(BlueprintReadOnly, Category = "AWS info")
		FTimerHandle RetrieveNewTokensHandle;
	UPROPERTY(BlueprintReadOnly, Category = "AWS info")
		FTimerHandle GetResponseTimeHandle;
	UPROPERTY(BlueprintReadOnly, Category = "AWS info")
		FTimerHandle PartyInfoHandle;
	TDoubleLinkedList<float> PlayerLatencies;
	UPROPERTY(BlueprintReadOnly, Category = "AWS info")
		FString JoinTicketId = "";
	FHttpModule* HttpModule;
	UPROPERTY(BlueprintReadOnly, Category = "AWS info")
		FString ApiUrl;
	UPROPERTY(BlueprintReadOnly, Category = "AWS info")
		FString RegionCode;

	// AWS Dedicated Server Level Name
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Server Level Option")
		FString GameLiftLevelName = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Server Level Option")
		FString GameLiftLevelNameOptions = "";

	// Pet info
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Partner")
		TArray<ESkillID> PartnerSkillArray;

	// inGame end time
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Exit Level")
		double endTime{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Exit Level")
		bool endGame{};

public:
	// 
	virtual void Init() override;
	UBaseInstance();
	UFUNCTION(BlueprintCallable, Category = "Reset Data")
		void resetGameInstance();
	virtual void Shutdown() override;

	//Replicated
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	UFUNCTION(BlueprintCallable, Category = "Socket")
		bool ConnectToServer();

	UFUNCTION(BlueprintCallable, Category = "Socket")
		bool SendAccessToken();

	UFUNCTION()
		void SetCognitoTokens(FString NewAccessToken, FString NewIdToken, FString NewRefreshToken);

	UFUNCTION(BlueprintCallable, Category = "Exit Level")
		double getElapseTime();

	UFUNCTION(BlueprintCallable, Category = "LeaveParty")
		bool LeaveParty();

	UFUNCTION(BlueprintCallable, Category = "LogoutGame")
		bool LogoutGame();

private:
	UFUNCTION()
		void RetrieveNewTokens();

	void OnRetrieveNewTokensResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void OnGetResponseTimeResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

};
