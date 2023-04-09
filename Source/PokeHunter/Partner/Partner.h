// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GenericTeamAgentInterface.h"
#include "PokeHunter/Base/SkillData.h"
#include "PokeHunter/Base/ItemInteractInterface.h"
#include "Partner.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnMontageEndDelegate);


UENUM(BlueprintType)
enum class EPartnerState : uint8
{
	//Stay State
	Posing,
	Unselected,
	MoveTarget,

	//Leave State
	Rushing,
	Howling,
	IceShard,
	MakingStorm
};

UCLASS()
class POKEHUNTER_API APartner : public ACharacter, public IGenericTeamAgentInterface, public IItemInteractInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APartner();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class AHunter* Hunter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UPartnerAnimInstance* PartnerAnim;

	FOnMontageEndDelegate OnMontageEnd;

	EPartnerState CurState;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FVector TargetPos;
	FVector LookTargetVec;
	FVector AttackPoint;
	AActor* Target;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	float HP{ 100.f};
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Status")
	float HealPerSecondAmount{1.f};
	bool bPosing;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bOrdered;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bUsingSkill;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Battle")
	bool bGrabbed;

	//TeamID
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle")
	FGenericTeamId TeamID;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//Perception
	FGenericTeamId GetGenericTeamId()const override;

	//Animation
	UFUNCTION(Server, Reliable)
	void ServerPlayMontage(FName Section);
	UFUNCTION(NetMulticast, Reliable)
	void MultiPlayMontage(FName Section);
	

	UFUNCTION()
	virtual void Attack();
	virtual void Howling();
	virtual void StopSkill();
	virtual void UseNormalSkill(ESkillID SkillID);
	virtual void UseSpecialSkill(ESkillID SkillID);
	inline void SetTarget(AActor* setTarget) { Target = setTarget;  };

	void FollowHunter(class AHunter* Hunter);

	//AnimationFunction
	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	//ItemInteractInterface
	virtual void InteractHealArea_Implementation();
	virtual void OutHealArea_Implementation();

};
