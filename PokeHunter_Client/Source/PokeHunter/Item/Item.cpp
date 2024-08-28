// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include "ItemData.h"
#include "Components/StaticMeshComponent.h"
#include "PokeHunter/Hunter/Hunter.h"


// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SceneRootComponent = CreateDefaultSubobject<USceneComponent>("My Scene Component");
	SetRootComponent(SceneRootComponent);

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(SceneRootComponent);

	StaticMesh->SetCollisionProfileName("WorldDynamic");

}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AItem::UseItem_Implementation(APawn* ItemOwner)
{

}

void AItem::UseItem(APawn* ItemOwner, FVector InitialPos, FVector EndPos)
{

}

void AItem::MultiDestroy_Implementation()
{
	Destroy();
}

void AItem::ServerDestroy_Implementation()
{
	MultiDestroy();
}

void AItem::ApplyAbillity_Implementation(AActor* OtherActor, UPrimitiveComponent* OtherComponent)
{

}