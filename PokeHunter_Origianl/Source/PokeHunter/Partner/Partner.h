// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GenericTeamAgentInterface.h"
#include "Net/UnrealNetwork.h"
#include "PokeHunter/Base/SkillData.h"
#include "PokeHunter/Base/ItemInteractInterface.h"
#include "PokeHunter/Base/EnemyInteractInterface.h"
#include "PokeHunter/Base/PartnerSkillInterface.h"
#include "PokeHunter/Base/BaseInstance.h"
#include "PokeHunter/Base/BaseCharacter.h"
#include "Partner.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnMontageEndDelegate);


UENUM(BlueprintType)
enum class EPartnerState : uint8
{
	//Stay State
	Posing,
	Unselected,
	MoveTarget,
	Knockback,

	//Leave State
	Rushing,
	SlashAttack,
	Howling,
	IceShard,
	MakingStorm,
	IceBreath
};

UCLASS()
class POKEHUNTER_API APartner : public ABaseCharacter, public IGenericTeamAgentInterface, public IItemInteractInterface, public IEnemyInteractInterface , public IPartnerSkillInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
		EPartnerType Type = EPartnerType::NonePartner;

	// Sets default values for this character's properties
	APartner();

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, Category = "AI")
	class AHunter* Hunter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UPartnerAnimInstance* PartnerAnim;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Anim")
	UAnimMontage* curMontage;
	
	FOnMontageEndDelegate OnMontageEnd;

	UBaseInstance* baseinstace;

	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadWrite)
	FVector TargetPos;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	FVector LookTargetVec;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	FVector AttackPoint;
	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadOnly, Category = "AI")
	class ACharacter* Target;
	
	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadOnly, Category = "AI")
	EPartnerState CurState;
	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadOnly, Category = "AI")
	bool bPosing;
	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadOnly, Category = "AI")
	bool bOrdered;
	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadOnly, Category = "AI")
	bool bUsingSkill;
	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadOnly, Category = "AI");
	bool bDied;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Battle")
	bool bGrabbed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Battle")
	float SlashDamage{ 10 };

	//TeamID
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle")
	FGenericTeamId TeamID;

	//Effect
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Effect")
	class UNiagaraSystem* SlashHitEffect;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Effect")
	class UNiagaraSystem* SlashMoveEffect;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Effect")
	float MoveEffectOffsetX;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

	//Perception
	FGenericTeamId GetGenericTeamId()const override;

	//Animation
	UFUNCTION(Server, Reliable)
	void ServerPlayMontage(FName Section);
	UFUNCTION(NetMulticast, Reliable)
	void MultiPlayMontage(FName Section);

	UFUNCTION(BlueprintCallable)
	inline EPartnerType GetType() { return Type; }

	// Status
	UFUNCTION(BlueprintCallable)
	inline void SetHP(float ChangedHP) { HP = ChangedHP;};
	UFUNCTION(BlueprintCallable)
	float GetHP() { return HP; };
	UFUNCTION(BlueprintCallable)
		void ResetStatus(FVector startPos) {
		StopAnimMontage(curMontage); 
		HP = 100.0f; CurState = EPartnerState::MoveTarget;
		TargetPos = startPos;
		bDied = false;
	}

	UFUNCTION(BlueprintCallable)
	bool CheckFalling();

	UFUNCTION()
	virtual void Attack();
	virtual void SlashAttack();
	UFUNCTION(BlueprintCallable)
	virtual void SlashMove();
	virtual void Howling();
	virtual void StopSkill();
	virtual void UseNormalSkill(ESkillID SkillID);
	virtual void UseSpecialSkill(ESkillID SkillID);
	virtual void CancelOrder();
	void SetTarget(ACharacter* setTarget);
	void FollowHunter(class AHunter* Hunter);

	//AnimationFunction
	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	//EnemyInterface
	virtual void InteractAttack_Implementation(FVector HitDirection, float Damage);
	virtual void InteractEarthquake_Implementation();
	virtual void InteractGrabAttack_Implementation();
	virtual void InteractWideAttack_Implementation(float Damage);

	//ItemInteractInterface
	virtual void InteractPotion_Implementation(float HealAmount);
	virtual void InteractHealArea_Implementation();
	virtual void OutHealArea_Implementation();


	//Replication
	UFUNCTION(Server, Reliable)
	void ServerStartPartnerInvincibility();
	UFUNCTION(NetMulticast, Reliable)
	void MultiStartPartnerInvincibility();

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ServerApplyDamage(AActor* DamagedActor, float DamageAmount, AController* EventInstigator, AActor* DamageCauser);
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void MultiApplyDamage(AActor* DamagedActor, float DamageAmount, AController* EventInstigator, AActor* DamageCauser);

	UFUNCTION(Server, Reliable)
	void ServerStopMontage(APartner* Partner);
	UFUNCTION(NetMulticast, Reliable)
	void MultiStopMontage(APartner* Partner);


	UFUNCTION(Server, Reliable)
	void ServerSetPosition(const FVector& LocVec);
	UFUNCTION(NetMulticast, Reliable)
	void MultiSetPosition(const FVector& LocVec);
	UFUNCTION(NetMulticast, Reliable)
	void MultiSetHunter(class AHunter* OwnerHunter);
	UFUNCTION(NetMulticast, Reliable)
	void MultiUseNormalSkill(ESkillID SkillID);
	UFUNCTION(NetMulticast, Reliable)
	void MultiUseSpecialSkill(ESkillID SkillID);
	UFUNCTION(NetMulticast, Reliable)
	void MultiCancelOrder();

	UFUNCTION(Server, Reliable)
	void ServerSpawnProjectile(APartner* OwnerPartner, TSubclassOf<class APartnerProjectile> SpawnProjectileClass, FVector StartLoc, FVector EndLoc, FRotator Rotation);
	UFUNCTION(Server, Reliable)
	void ServerSpawnEffect(class UNiagaraSystem* Niagara, const FVector& SpawnLoc);
	UFUNCTION(NetMulticast, Reliable)
	void MultiSpawnEffect(class UNiagaraSystem* Niagara, const FVector& SpawnLoc);

};
