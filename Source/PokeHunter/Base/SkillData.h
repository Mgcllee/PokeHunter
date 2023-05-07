// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SkillData.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class ESkillID : uint8
{
	None = 0,
	�޸��� = 201,
	�Ͽ︵ = 202,
	������� = 203,
	����â = 211,
	�ñ���ǳ = 212,
	�ñ���� = 213,
	�������� = 221,
	���׿� = 222
};

USTRUCT(BlueprintType)
struct FSkillInfo
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill")
	ESkillID ID {};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill")
	FName Name {};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill")
	FName Type {};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill")
	float CoolTime{5.f};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill")
	FText ToolTip {};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill")
	UTexture2D* Icon = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Skill")
	float CurTime{ 0.f };

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Skill")
	bool bReady{ true };

	void CheckTime(float DeltaTime)
	{
		CurTime += DeltaTime;
		if (CurTime >= CoolTime)
		{
			CurTime = CoolTime;
			bReady = true;
		}
	}

	void UsedSkill()
	{
		CurTime = 0.f;
		bReady = false;
	}

	bool CheckReady()
	{
		return bReady;
	}
};


UCLASS(Blueprintable, BlueprintType)
class POKEHUNTER_API USkillData : public UObject
{
	GENERATED_BODY()
	
public:
	USkillData();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSkillInfo SkillInfo;


};
