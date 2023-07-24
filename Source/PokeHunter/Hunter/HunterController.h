// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PokeHunter/PokeHunter.h"
#include "GameFramework/PlayerController.h"
#include "HunterController.generated.h"

/**
 * 
 */
UCLASS()
class POKEHUNTER_API AHunterController : public APlayerController
{
	GENERATED_BODY()
	
protected:

public:
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;

	UFUNCTION()
		void SetCharacterMaterialFromGameInstance(APawn* ClientPawn);

	// ������ ĳ������ ��Ƽ���� ������ �����ϴ� �Լ�
	UFUNCTION(Server, Reliable)
		void Server_SetCharacterMaterialInfo(APawn* ClientPawn, int32 NewMaterialInfo);
	UFUNCTION(Server, Reliable)
		void ClientConnectToServer(int32 NewMaterialInfo);
};
