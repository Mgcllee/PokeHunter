// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PokeHunter/Item/ItemData.h"
#include "InventoryComponent.generated.h"



UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class POKEHUNTER_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInventoryComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FItemCnter> InfoArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FItemCnter> FarmingArray;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class AHunter* Hunter;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int capacity = 24;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	bool AddItemData(FItemCnter ItemCnter);
	UFUNCTION(BlueprintCallable)
	bool AddItemInfo(FName ItemID, int Cnt);

	UFUNCTION(BlueprintCallable)
	void ChangeSlot(AActor* Storage, FName TargetName, int TargetIndex, FName GoalName, int GoalIndex);
	UFUNCTION(BlueprintCallable)
	void SwapSlot(int TargetIndex, int GoalIndex);
	UFUNCTION(BlueprintCallable)
	int GetItemCnt(FName id);
	
};
