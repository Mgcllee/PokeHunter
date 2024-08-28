// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "..\PokeHunter.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GenericTeamAgentInterface.h"
#include "PokeHunter/Item/ItemData.h"
#include "PokeHunter/Base/SkillData.h"
#include "PokeHunter/Base/ItemInteractInterface.h"
#include "PokeHunter/Base/EnemyInteractInterface.h"
#include "PokeHunter/Base/BaseInstance.h"
#include "PokeHunter/Base/BaseCharacter.h"
#include "Hunter.generated.h"

//Dynamic 
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDynamicDele);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDynamicDeleParam, float, val);

// Particle
class UParticleSystemComponent;

UENUM(BlueprintType)
enum class EPlayerState : uint8
{
	Idle,
	Dive,
	Zoom,
	Install,
	Drink,
	Hit,
	Knockback,
	Die
};

USTRUCT(BlueprintType)
struct FHunterInfo
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Status")
	int32 HunterNum {};

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, category = "Status")
	bool bCanCombat = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Partner Skill")
	TArray<ESkillID> PartnerSkillArray;
};

USTRUCT(BlueprintType)
struct FPartyInfo
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "partyMemberInfo")
		FName hunterName;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "partyMemberInfo")
		int32 hunterDamage;
};

UCLASS()
class POKEHUNTER_API AHunter : public ABaseCharacter, public IGenericTeamAgentInterface, public IItemInteractInterface, public IEnemyInteractInterface
{
	GENERATED_BODY()

public:

	//Camera Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class UCameraComponent* FollowCamera;
	//Inventory Component
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Inventory")
	class UInventoryComponent* Inventory{};

	//

	//HunterInfo
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Status")
	FHunterInfo HunterInfo;

	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadWrite, Category = "Status")
	float HunterStamina{ 100 };

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Status")
	float DiveSpeed{ 1000.f };
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Status")
	float SprintSpeed{ 700.f };
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Status")
	float WalkSpeed{ 500.f };
	
	//
	UPROPERTY(VisibleAnywhere, Replicated,  BlueprintReadOnly, Category = "Interaction")
	class AInteractActor* InteractingActor;

	//QuickSlot
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "QuickSlot")
	TArray<FItemCnter> QuickSlotArray;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "QuickSlot")
	int CurQuickKey{};

	//Item
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
	class AItem* CurItem;

	//UI
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf <UUserWidget> MainUIClass;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "UI")
	class UUserWidget* MainUI;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf <UUserWidget> StorageUIClass;
	class UUserWidget* StorageUI;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf <UUserWidget> PauseUIClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	class UUserWidget* PauseUI;

	//Partner
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Partner")
	EPartnerType PartnerType {EPartnerType::WolfPartner};
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Partner")
	class APartner* Partner{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Partner")
	TArray<FSkillInfo> SkillInfoArray;

	//Delegate
	UPROPERTY(BlueprintAssignable, VisibleAnywhere, BlueprintCallable, BlueprintReadWrite)
	FDynamicDeleParam FMouseWheelDelegate;
	UPROPERTY(BlueprintAssignable, VisibleAnywhere, BlueprintCallable)
	FDynamicDele FIKeyDelegate;

	//Timeline
	FTimeline DiveTimeline;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Timeline")
	UCurveFloat* DiveCurve;
	FOnTimelineFloat DiveInterpCallback;
	float LastSpeed;
	FVector LastInput;

	//Camera Variable
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float ArmLengthTo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float CameraZoomTo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float ArmSpeed;

	//PlayerState
	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadOnly, Category = "Animation")
	EPlayerState CurState {EPlayerState::Idle};

	//Animation
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	class UHunterAnimInstance* HunterAnim;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* curMontage;

	//Bool
	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadOnly, Category = "Animation")
	bool bUpperOnly;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Partner")
	bool bPartnerMode;
	
	//TeamID
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle")
	FGenericTeamId TeamID;

	//Battle
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite)
	bool bShiftDown{ false };

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Battle")
	float ReloadTime{};
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Battle")
	float StartShotTime;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Battle")
	bool bCanShot;
	

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Battle")
	float StaminaPerSecondAmount;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Battle")
	float StaminaDecreaseAmount;
	int SaveSecond;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Battle")
	bool bGrabbed;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Battle")
	bool bBound;

	//Effect
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Effect")
	class UNiagaraSystem* HealEffect;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Battle")
	float bTotalDamaged = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Battle")
	float bPartnerTotalDamaged = 0.0f;

protected:
	// Sets default values for this character's properties
	AHunter();
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UBaseInstance* gameinstance = nullptr;

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

	//Perception
	FGenericTeamId GetGenericTeamId()const override;

	//Replicated
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
	UFUNCTION(Server, Reliable)
	void ServerPlayMontage (AHunter* Hunter, FName Session);
	UFUNCTION(NetMulticast, Reliable)
	void MultiPlayMontage(AHunter* Hunter, FName Session);

	UFUNCTION(Server, Reliable)
	void ServerSprint(AHunter* Hunter, bool bSprinting);
	UFUNCTION(NetMulticast, Reliable)
	void MultiSprint(AHunter* Hunter, bool bSprinting);

	UFUNCTION(Server, Reliable)
	void ServerRoll(AHunter* Hunter, const FVector& LastInputVec);
	UFUNCTION(NetMulticast, Reliable)
	void MultiRoll(AHunter* Hunter, const FVector& LastInputVec);

	UFUNCTION(Server, Reliable)
	void ServerZoom(AHunter* Hunter, bool bZoom);
	UFUNCTION(NetMulticast, Reliable)
	void MultiZoom(AHunter* Hunter, bool bZoom);

	UFUNCTION(Server, Reliable)
	void ServerStartInvincibility();
	UFUNCTION(NetMulticast, Reliable)
	void MultiStartInvincibility();
	UFUNCTION(BlueprintCallable)
	void StartInvincibility();
	void SetInstallMode();

	//Partner RPC
	UFUNCTION(Server, Reliable)
	void ServerSpawnPartner(AHunter* OwnerHunter, TSubclassOf <APartner> SpawnPartnerClass, const FVector& SpawnLoc);
	UFUNCTION(NetMulticast, Reliable)
	void MultiSetPartner(APartner* NewPartner);
	UFUNCTION(Server, Reliable)
	void ServerSetPartnerPosition(APartner* MyPartner, const FVector& LocVec);
	UFUNCTION(NetMulticast, Reliable)
	void MultiSetPartnerPosition(const FVector& LocVec);
	UFUNCTION(Server, Reliable)
	void ServerUsePartnerNormalSkill(APartner* MyPartner, ESkillID SkillID);
	UFUNCTION(Server, Reliable)
	void ServerUsePartnerSpecialSkill(APartner* MyPartner, ESkillID SkillID);
	UFUNCTION(Server, Reliable)
	void ServerCancelOrder(APartner* MyPartner);

	//Use Item RPC
	UFUNCTION(Server, Reliable)
	void ServerSpawnItem(AHunter* OwnerHunter, TSubclassOf<AItem> SpawnItemClass, FVector StartLoc, FVector EndLoc, FRotator Rotation);
	UFUNCTION(Server, Reliable)
	void ServerSpawnBullet(AHunter* OwnerHunter, TSubclassOf<AItem> SpawnItemClass, FVector StartLoc, FVector EndLoc, FRotator Rotation);
	UFUNCTION(Server, Reliable)
	void ServerShotBullet(ABullet* Bullet, AHunter* OwnerHunter, FVector InitialPos, FVector EndPos);
	UFUNCTION(Server, Reliable)
	void	ServerUsePotion(AHunter* OwnerHunter, TSubclassOf<AItem> SpawnItemClass);
	UFUNCTION(NetMulticast, Reliable)
	void	MultiUsePotion(APotion* Potion);
	UFUNCTION(Server, Reliable)
	void ServerInteractObject(AInteractActor* TargetActor, AHunter* OwnerHunter);
	UFUNCTION(NetMulticast, Reliable)
	void MultiInteractObject(AHunter* OwnerHunter);
	UFUNCTION(NetMulticast, Reliable)
	void ServerSpawnEffect(class UNiagaraSystem* Niagara, const FVector& SpawnLoc);
	UFUNCTION(NetMulticast, Reliable)
	void MultiSpawnEffect(class UNiagaraSystem* Niagara, const FVector& SpawnLoc);

	UFUNCTION(BlueprintNativeEvent)
	void UpdateQuickSlot();

	UFUNCTION(BlueprintCallable)
	void DrinkPotion();

	//Status
	UFUNCTION(BlueprintCallable)
	void SetHP(float setHP) { HP = setHP; }
	UFUNCTION(BlueprintCallable)
	float GetHP() { return HP; };
	UFUNCTION(BlueprintCallable)
	void SetStamina(float Stamina);
	UFUNCTION(BlueprintCallable)
	float GetStamina() { return HunterStamina; }
	UFUNCTION(BlueprintCallable)
	void ResetStatus() { StopAnimMontage(curMontage); HP = 100.0f, HunterStamina = 100.0f, CurState = EPlayerState::Idle; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Party member info")
		TMap<FName, float> PartyMemberHP;		//  = { {FName("Tester01"), 0.f}, {FName("Tester02"), 80.f}, {FName("Tester03"), 20.f}, {FName("Tester04"), 60.f} };	// <OwnerName, pet HP>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Party member info")
		TMap<FName, float> PartyMemberPetHP;	// = { {FName("Tester01"), 0.f}, {FName("Tester02"), 80.f}, {FName("Tester03"), 20.f}, {FName("Tester04"), 60.f} };	// <OwnerName, pet HP>

	UFUNCTION(Server, Reliable, Category = "Party member info")
		void ServerHunterHP(FName PlayerName, float NewHP);
	UFUNCTION(NetMulticast, Reliable, Category = "Party member info")
		void MultiHunterHP(FName PlayerName, float NewHP);
	UFUNCTION(Server, Reliable, Category = "Party member info")
		void ServerPetHP(FName PlayerName, float NewHP);
	UFUNCTION(NetMulticast, Reliable, Category = "Party member info")
		void MultiPetHP(FName PlayerName, float NewHP);

	UFUNCTION(Server, Reliable)
	void ServerSetMaxSpeed(AHunter* OwnerHunter, float NewSpeed);
	UFUNCTION(NetMulticast, Reliable)
	void MultiSetMaxSpeed(AHunter* OwnerHunter, float NewSpeed);

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	UFUNCTION(BlueprintCallable)
	void SpaceDown();
	UFUNCTION(BlueprintCallable)
	void LShiftDown();
	UFUNCTION(BlueprintCallable)
	void LShiftUp();
	void LMBDown();
	UFUNCTION(BlueprintCallable)
	void RMBDown();
	UFUNCTION(BlueprintCallable)
	void RMBUp();
	UFUNCTION(BlueprintCallable)
	void WheelInput(float Val);
	UFUNCTION(BlueprintCallable)
	void ChangeQuickslot(float Val);
	UFUNCTION(BlueprintCallable)
	void SetQuickslot(FName ItemID, int index);
	UFUNCTION(BlueprintCallable)
	void OpenInventory();
	UFUNCTION(BlueprintCallable)
	void EKeyDown();
	UFUNCTION(BlueprintCallable)
	void RKeyDown();
	UFUNCTION(BlueprintCallable)
	void ESCKeyDown();
	UFUNCTION(BlueprintCallable)
	void CtrlDown();
	UFUNCTION(BlueprintCallable)
	void CtrlUp();
	UFUNCTION(BlueprintCallable)
	void Use1Skill();
	UFUNCTION(BlueprintCallable)
	void Use2Skill();
	UFUNCTION(BlueprintCallable)
	void Use3Skill();
	UFUNCTION(BlueprintCallable)
	void Use4Skill();

	//Partner
	UFUNCTION(BlueprintCallable)
	void SetPartnerSkill(TArray<ESkillID> SkillArray, int SkillListNum);
	UFUNCTION(BlueprintCallable)
	void SetPartnerTarget(ACharacter* setTarget);
	void SetPartner(class APartner* SelectedPartner);
	bool SuccessUseSkill(ESkillID SkillID);
	bool CheckUseSkill(ESkillID SkillID);
	UFUNCTION(BlueprintNativeEvent)
	void UpdateSkillSlots();

	//Collision Function
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	//Animation Function
	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	//Timeline Function
	UFUNCTION()
	void DiveInterpReturn(float Value);
	void SetDiveCurveTime(float length);

	//ItemInterface Function
	virtual void InteractHealArea_Implementation();
	virtual void OutHealArea_Implementation();
	virtual void InteractPotion_Implementation(float HealAmount);

	//EnemyInterface Function
	virtual void InteractEarthquake_Implementation();
	virtual void InteractAttack_Implementation(FVector HitDirection, float Damage);
	virtual void InteractGrabAttack_Implementation();
	virtual void InteractWideAttack_Implementation(float Damage);

	//Hunter New Material
	UFUNCTION(Server, Reliable, Category = "Material")
		void ServerChangeMaterialIndex(int32 NewMaterialIndex);
	UFUNCTION(NetMulticast, Reliable, Category = "Material")
		void MulticastChangeMaterialIndex(int32 NewMaterialIndex);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ResultUIDamage")
		FName HunterName;

	UFUNCTION(Server, Reliable, Category = "ResultUIDamage")
		void ServerGetALLNames(FName newHunterName);
	UFUNCTION(NetMulticast, Reliable, Category = "ResultUIDamage")
		void MulticastGetALLNames(FName newName);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ResultUIDamage")
		TArray<FName> huntersName;

	UFUNCTION(Server, Reliable, Category = "ResultUIDamage")
		void ServerGetALLDamage(AHunter* hunter, int amountDamage);
	UFUNCTION(NetMulticast, Reliable, Category = "ResultUIDamage")
		void MulticastGetALLDamage(FName name, int amountDamage);
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "ResultUIDamage")
		TArray<FPartyInfo> DamageList;

protected:
	UPROPERTY()
		UMaterialInterface* Material;

public:	// Particle System
	UPROPERTY(EditDefaultsOnly, Category = "Particle")
		UParticleSystemComponent* Heal_Effect = nullptr;

private:
	// Character Movement Input
	void MoveForward(float Val);
	void MoveRight(float Val);
	void LookUp(float NewAxisValue);
	void Turn(float NewAxisValue);
};
