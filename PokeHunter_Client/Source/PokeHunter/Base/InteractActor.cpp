// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractActor.h"
#include "Net/UnrealNetwork.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"

// Sets default values
AInteractActor::AInteractActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;

	SceneRootComponent = CreateDefaultSubobject<USceneComponent>("My Scene Component");
	SetRootComponent(SceneRootComponent);

	//Collision
	InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractSphere"));
	InteractionSphere->AddLocalOffset(FVector(0.f, 0.f, GetSimpleCollisionHalfHeight()));
	InteractionSphere->SetSphereRadius(50.f);
	InteractionSphere->SetupAttachment(SceneRootComponent);

	
}

// Called when the game starts or when spawned
void AInteractActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AInteractActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AInteractActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AInteractActor, Master);
}

void AInteractActor::Interact_Implementation(AHunter* Hunter)
{
	Master = Hunter;
}

void AInteractActor::ServerInteract_Implementation(AHunter* Hunter)
{
	SetActorTickEnabled(true);
	MultiInteract(Hunter);
}

void AInteractActor::MultiInteract_Implementation(AHunter* Hunter)
{
	Interact_Implementation(Hunter);
}
