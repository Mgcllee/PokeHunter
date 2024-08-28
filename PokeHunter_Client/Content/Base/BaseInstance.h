// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Runtime/Core/Public/Windows/AllowWindowsPlatformTypes.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "Runtime/Networking/Public/Networking.h"
#include "Runtime/Networking/Public/Interfaces/IPv4/IPv4Address.h"
#include "Runtime/Sockets/Public/Sockets.h"
#include "Runtime/Sockets/Public/SocketSubsystem.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "../../../../PH-Server/IOCPServer/protocol.h"
#include "Windows/WindowsPlatformMisc.h"
#include "Runtime/Core/Public/Windows/HideWindowsPlatformTypes.h"
#include "PokeHunter/Item/ItemData.h"

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
	UBaseInstance();

	virtual void Init() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Player_Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Player_Skin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Pet_Number;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Quick_Item;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Quick_Skill;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EPartnerType myPartner = EPartnerType::NonePartner;

	FSocket* gSocket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(TEXT("Stream"), TEXT("Client Socket"));
	TSharedRef<FInternetAddr>addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	FIPv4Address ip{};
	int reval;

	UFUNCTION(BlueprintCallable, Category = "Socket")
		bool ConnectToServer(FString server_addr ="172.30.160.1"); // Default Addr = 172.30.160.1

	UFUNCTION(BlueprintCallable, Category = "Socket")
		bool SendAccessToken();

	virtual void Shutdown() override;

	UPROPERTY()
		FString AccessToken;

	UPROPERTY()
		FString IdToken;

	UPROPERTY()
		FString RefreshToken;

	UPROPERTY()
		FTimerHandle RetrieveNewTokensHandle;

	UPROPERTY()
		FTimerHandle GetResponseTimeHandle;

	UPROPERTY()
		FTimerHandle PartyInfoHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FItemCnter> InstanceInfoArray;

	TDoubleLinkedList<float> PlayerLatencies;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString GameLiftLevelName = "SurvivalArea";

	UFUNCTION()
		void SetCognitoTokens(FString NewAccessToken, FString NewIdToken, FString NewRefreshToken);

	UFUNCTION(BlueprintCallable, Category = "Home")
		bool returnMyHome();

	UPROPERTY()
		FString JoinTicketId;

	FHttpModule* HttpModule;


	UPROPERTY()
		FString ApiUrl;

	UPROPERTY()
		FString RegionCode;

private:
	UFUNCTION()
		void RetrieveNewTokens();

	void OnRetrieveNewTokensResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void OnGetResponseTimeResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TMap<FName, int> PartyListMap;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int PartnerNumber = -1;
};
