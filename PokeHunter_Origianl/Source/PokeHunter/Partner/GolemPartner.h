// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Partner.h"
#include "GolemPartner.generated.h"

/**
 * 
 */
UCLASS()
class POKEHUNTER_API AGolemPartner : public APartner
{
	GENERATED_BODY()

	AGolemPartner();

	TSubclassOf<class APartnerProjectile> IceShardClass;
	
public:
	virtual void UseSpecialSkill(ESkillID SkillID);
	void UseMeteor();
	void ThrowRock();

};
