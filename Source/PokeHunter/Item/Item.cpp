// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include "ItemData.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "PokeHunter//Hunter/Hunter.h"
#include "PokeHunter/Hunter/InventoryComponent.h"

// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Mesh
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(GetRootComponent());

	//Collision
	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	CollisionSphere->SetupAttachment(GetRootComponent());
	CollisionSphere->SetSphereRadius(100.f);

	//Data
	static ConstructorHelpers::FClassFinder<UItemData>TempClass(TEXT("/Game/Item/Blueprint/BP_DefaultItemData"));
	if (TempClass.Succeeded())
	{
		ItemDataClass = TempClass.Class;
	}


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

void AItem::Interact_Implementation(AHunter* Hunter)
{
	Hunter->Inventory->AddItem(ItemDataClass);
	this->Destroy();
}