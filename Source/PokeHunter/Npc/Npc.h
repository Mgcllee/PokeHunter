// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NpcInterface.h"
#include "Npc.generated.h"


UCLASS()
class POKEHUNTER_API ANpc : public AActor, public INpcInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANpc();

	//��ȣ�ۿ�
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
		class USphereComponent* InteractionSphere;

	//�޽�
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
		UStaticMeshComponent* StaticMesh;

	//�浹
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
		class UBoxComponent* CollisionBox;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void interact_Implementation() override;

};
