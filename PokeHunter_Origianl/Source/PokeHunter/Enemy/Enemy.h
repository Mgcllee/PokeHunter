// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GenericTeamAgentInterface.h"
#include "PokeHunter/Base/PartnerSkillInterface.h"
#include "PokeHunter/Base/ItemInteractInterface.h"
#include "PokeHunter/Base/EnemyInteractInterface.h"
#include "Enemy.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnMontageEndDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDamageDelegate, float, DamageAmount, FVector, HitLoc);

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	Patrol UMETA(DisplayName = "Patrol"),
	Chase UMETA(DisplayName = "Chase"),
	Return UMETA(DisplayName = "Return"),
	Hit UMETA(DisplayName = "Hit"),
	Grogy UMETA(DisplayName = "Grogy"),
	Die UMETA(DisplayName = "Die"),
	Roar UMETA(DisplayName = "Roar"),
	Attention UMETA(DisplayName = "Attention"),
	PatternAttack UMETA(DisplayName = "PatternAttack"),
	NormalAttack UMETA(DisplayName = "NormalAttack"),
	JumpAttack UMETA(DisplayName = "JumpAttack"),
	LongAttack UMETA(DisplayName = "LongAttack"),
	ChargeAttack UMETA(DisplayName = "ChargeAttack"),
	WideAttack UMETA(DisplayName = "WideAttack"),
	RushAttack UMETA(DisplayName = "RushAttack"),
	LeftDestroy UMETA(DisplayName = "LeftDestroy"),
	RightDestroy UMETA(DisplayName = "RightDestroy")
	
};


UCLASS()
class POKEHUNTER_API AEnemy : public ACharacter, public IGenericTeamAgentInterface, public IItemInteractInterface, public IEnemyInteractInterface, public IPartnerSkillInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AEnemy();

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite)
		float HP{ 30 };

	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadOnly)
		class ABaseCharacter* Target;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class AActor* AgroTarget;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		bool bWaitingAgro;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "DropItem")
		TSubclassOf <class AInteractActor> DropItemBoxClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "DropItem")
		TArray<FName> DropItemID_Array;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		TArray<class ABaseCharacter*> TargetArray;

	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadOnly)
		FVector TargetPos;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UEnemyAnimInstance* EnemyAnim;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class USkeletalMeshComponent* SubSkeletalMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class UEnemyAnimInstance* EnemySubAnim;


	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadWrite)
		EEnemyState CurState {
		EEnemyState::Patrol
	};

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		TSubclassOf <class AEnemyProjectile> ProjectileClass;

	FOnMontageEndDelegate OnMontageEnd;
	UPROPERTY(BlueprintAssignable, VisibleAnywhere, BlueprintCallable)
		FOnDamageDelegate OnDamage;

	//TeamID
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle")
		FGenericTeamId TeamID;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float BaseWalkSpeed {};

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		FVector BaseLocation;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float MoveRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float AttackDamage = 10.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float AttackRange = 200.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float EarthquakeRange = 1000.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		float ReflectDamgeAmount{ 0.f };
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		bool bReflecting{ false };

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		float StartBindingTime;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float BindingTime;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		bool bBinding{ false };

	//상태 이상
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float SavedDamage{ 0 };
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float GrogyDamage{};
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		bool bGrogy{ false };
	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadWrite)
		bool bPoisoned{ false };
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float PoisonedTime{};
	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadWrite)
		float PoisonLimitTime;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_Burning, BlueprintReadWrite)
		bool bBurning{ false };
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float BurningTime{};
	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadWrite)
		float BurningLimitTime;

	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadWrite);
	bool bFrozen{false};
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float FrozenTime{};
	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadWrite)
	float FrozenLimitTime{};
	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadWrite)
	bool bSlow{false};
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float SlowTime{};
	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadWrite)
	float SlowLimitTime{};

	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadWrite);
	bool bWeaken{ false };


	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadWrite)
	bool bDied{ false };

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const override;

	FGenericTeamId GetGenericTeamId()const override;


	//CollisionFunction
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable)
	bool IsJumping();

	//Replication
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const;

	UFUNCTION(Server, Reliable)
	void ServerPlayMontage(AEnemy* Enemy, FName Section);
	UFUNCTION(NetMulticast, Reliable)
	void MultiPlayMontage(AEnemy* Enemy, FName Section);
	UFUNCTION(Server, Reliable)
	void ServerStartBinding();
	UFUNCTION(NetMulticast, Reliable)
	void MultiStartBinding();
	UFUNCTION(BlueprintCallable)
	void StartBinding();
	UFUNCTION(BlueprintCallable)
	void StartPoison();

	UFUNCTION(NetMulticast, Reliable)
	void OnRep_Burning();

	//Replication
	UFUNCTION(Server, Reliable)
	void ServerApplyPointDamage(AActor* OtherActor, float DamageAmount, FVector HitDirection, AActor* DamageCauser, const FHitResult& SweepResult);
	UFUNCTION(NetMulticast, Reliable)
	void MultiApplyPointDamage(AActor* OtherActor, float DamageAmount, FVector HitDirection, AActor* DamageCauser, const FHitResult& SweepResult);
	UFUNCTION(Server, Reliable)
	void ServerApplyDamage(AActor* OtherActor, float DamageAmount, AController* DamageInstigator, AActor* DamageCauser, TSubclassOf<UDamageType> DamageType);
	UFUNCTION(NetMulticast, Reliable)
	void MultiApplyDamage(AActor* OtherActor, float DamageAmount, AController* DamageInstigator, AActor* DamageCauser, TSubclassOf<UDamageType> DamageType);
	UFUNCTION(Server, Reliable)
	void ServerShowDamage(float DamageAmount, const FVector& ShowLoc);
	UFUNCTION(NetMulticast, Reliable)
	void MultiShowDamage(float DamageAmount, const FVector& ShowLoc);


	void SetTarget(AActor* NewTarget) { Target = Cast<class ABaseCharacter>(NewTarget); };
	void SeeNewTarget(AActor* NewTarget);
	void HearSound(FVector SoundLoc, AActor* AgroTarget);

	void ComeBackHome(float Distance);
	bool CheckInMoveRange();

	UFUNCTION(BlueprintCallable)
	void LeaveTarget(AActor* KilledTarget);
	UFUNCTION(BlueprintCallable)
	void ChangeTarget();
	UFUNCTION(BlueprintCallable)
	virtual int CheckInRange();
	UFUNCTION(BlueprintCallable)
	virtual int CheckPattern();

	UFUNCTION(BlueprintCallable)
	virtual void Attack(int AttackPattern);
	UFUNCTION(BlueprintCallable)
	virtual void PatternAttack(int AttackPattern);
	UFUNCTION(BlueprintCallable)
	virtual void LongAttack();
	UFUNCTION(BlueprintCallable)
	virtual void Roar();
	UFUNCTION(BlueprintCallable)
	virtual void Patrol();
	UFUNCTION(BlueprintCallable)
	virtual void JumpAttack();
	UFUNCTION(BlueprintCallable)
	virtual void ChargeAttack();
	UFUNCTION(BlueprintCallable)
	virtual void RushAttack();
	UFUNCTION(BlueprintCallable)
	virtual void WideAttack();
	UFUNCTION(BlueprintCallable)
	virtual void LaunchToTarget();
	UFUNCTION(BlueprintCallable)
	virtual void Block();
	UFUNCTION(BlueprintCallable)
	virtual void Reflect();
	UFUNCTION(BlueprintCallable)
	virtual void Die();
	UFUNCTION(BlueprintCallable)
	virtual void SpawnItem();
	

	//Replication
	UFUNCTION(Server, Reliable)
	void ServerSpawnItemBox(const FVector& SpawnLoc, TSubclassOf<AInteractActor> SpawnClass, const TArray<FName>&ItemID_Array);
	UFUNCTION(Server, Reliable)
	void ServerSpawnProjectile(TSubclassOf<class AEnemyProjectile>SpawnClass, const FVector& SpawnLoc, const FVector& EndLoc, const FVector& DirectionVec);
	UFUNCTION(Server, Reliable)
	void ServerDestroy();
	UFUNCTION(NetMulticast, Reliable)
	void MultiDestroy();


	//Animation Function
	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	//Interface
	virtual void InteractIce_Implementation();
	virtual void InteractFire_Implementation(UPrimitiveComponent* HitComponent);
	virtual void InteractBindTrap_Implementation();

	//EnemyInterface
	virtual void InteractEarthquake_Implementation();
	virtual void InteractAttack_Implementation(FVector HitDirection, float Damage);
	virtual void InteractGrabAttack_Implementation();

	//PartnerSkillInterface
	virtual void InteractIceSkill_Implementation();


};
