// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
//#include "PokeHunter/Hunter/Hunter.h"
//#include "PokeHunter/Hunter/HunterController.h"
#include "HunterState.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FU_DamageInfo {
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 hunterAmount {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 petAmount {};
};

USTRUCT(BlueprintType)
struct FU_HPInfo {
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Status")
		float hunterHP {};

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Status")
		float petHP {};
};

UCLASS()
class POKEHUNTER_API AHunterState : public APlayerState
{
	GENERATED_BODY()

public:
	UPROPERTY()
		FString PlayerSessionId;

	UPROPERTY()
		FString MatchmakingPlayerId;

	UPROPERTY(Replicated)
		FString Team;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hunter info")
		FName MyName {};

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Battle")
		float totalDamage;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Battle")
		FU_DamageInfo damageInfo {
		0, 0
	};

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Battle")
		FU_HPInfo hpInfo {
		0, 0
	};

public:
	void InitPlayerData();

	//Replication
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const;

	UFUNCTION(BlueprintCallable, Category = "Battle")
		FU_DamageInfo getTotalDamage()	{ return damageInfo; }
	UFUNCTION(BlueprintCallable, Category = "Battle")
		FU_HPInfo getHPInfo()			{ return hpInfo; }

	UFUNCTION(BlueprintCallable, Category = "Battle")
		void setHPInfoHunter(float newHP) { hpInfo.hunterHP = newHP; }
	UFUNCTION(BlueprintCallable, Category = "Battle")
		void setHPInfoPet(float newHP) { hpInfo.petHP = newHP; }

};
