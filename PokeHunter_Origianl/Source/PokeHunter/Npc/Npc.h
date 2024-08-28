// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PokeHunter/Base/InteractActor.h"
#include "PokeHunter/Hunter/Hunter.h"
#include "Blueprint/UserWidget.h"
#include "Npc.generated.h"


UCLASS()
class POKEHUNTER_API ANpc : public AInteractActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANpc();

	//Mesh
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Mesh")
	class USkeletalMeshComponent* SkeletalMesh;

	//충돌
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
	class UBoxComponent* CollisionBox;


	//카메라
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class UCameraComponent* FollowCamera;

	//UI
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<class UUserWidget> UIClass;

	//Sound
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sound")
	USoundBase* InteractSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sound")
	USoundBase* EndSound;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Interact_Implementation(AHunter* Hunter) override;
	void OpenUI();

public:
	class AHunter* Master;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bActive;
	FTimerHandle TimerHandle;
};
