// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PokeHunter/Base/InteractInterface.h"
#include "InteractActor.generated.h"

UCLASS()
class POKEHUNTER_API AInteractActor : public AActor, public IInteractInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractActor();

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* SceneRootComponent = nullptr;

	//상호작용
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	class USphereComponent* InteractionSphere;


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated)
	class AHunter* Master;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const;

	virtual void Interact_Implementation(AHunter* Hunter) override;

	UFUNCTION(Server, Reliable)
	void ServerInteract(AHunter* Hunter);
	UFUNCTION(NetMulticast, Reliable)
	void MultiInteract(AHunter* Hunter);
};
