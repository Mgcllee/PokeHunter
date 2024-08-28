// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PokeHunter/Base/InteractActor.h"
#include "Item.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
	Bullet = 0,
	Potion = 1,
	Trap = 2
};


UCLASS()
class POKEHUNTER_API AItem: public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* SceneRootComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class APawn* ThisOwner;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mesh")
	class UStaticMeshComponent* StaticMesh;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Type")
	EItemType ItemType; 

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintNativeEvent)
	void UseItem(APawn* ItemOwner);
	virtual void UseItem_Implementation(APawn* ItemOwner);
	virtual void UseItem(APawn* ItemOwner, FVector InitialPos, FVector EndPos);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ApplyAbillity(AActor* OtherActor, UPrimitiveComponent* OtherComponent);


	//Replicate
	UFUNCTION(Server, Reliable)
	void ServerDestroy();
	UFUNCTION(NetMulticast, Reliable)
	void MultiDestroy();
};
