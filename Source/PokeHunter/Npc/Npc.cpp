// Fill out your copyright notice in the Description page of Project Settings.


#include "Npc.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"

// Sets default values
ANpc::ANpc()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//�浹 �ڽ�
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	SetRootComponent(CollisionBox);

	//�޽�
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(GetRootComponent());

	//��ȣ�ۿ� ����(��)
	InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
	InteractionSphere->SetupAttachment(GetRootComponent());
	InteractionSphere->SetSphereRadius(100.f);

	

}

// Called when the game starts or when spawned
void ANpc::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ANpc::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ANpc::interact_Implementation()
{

}