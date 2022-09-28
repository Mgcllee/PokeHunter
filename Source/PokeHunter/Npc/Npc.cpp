// Fill out your copyright notice in the Description page of Project Settings.


#include "Npc.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "PokeHunter/Hunter/Hunter.h"

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

	//ī�޶�
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetOffset = FVector(0, 0, 0.0f);
	CameraBoom->TargetArmLength = 100.f;
	CameraBoom->SetWorldRotation(FRotator(0, 0, 180.0f));
	CameraBoom->bDoCollisionTest = false;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	//NpcUI = CreateWidget(GetWorld(), UIClass);
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

void ANpc::interact_Implementation(AHunter* Hunter)
{
	Master = Hunter;
	if (bActive)
	{
		Cast<APlayerController>(Master->GetController())->SetViewTargetWithBlend(Hunter, 1.0f);
		Master->StorageUI->RemoveFromViewport();
	}
	else
	{
		Cast<APlayerController>(Master->GetController())->SetViewTargetWithBlend(this, 1.0f);
	}
	Master->DisableInput(Cast<APlayerController>(Master->Controller));
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ANpc::OpenUI, 1.0f, false, 1.0f);
}

void ANpc::OpenUI()
{
	GetWorldTimerManager().ClearTimer(TimerHandle);
	if (bActive)
	{
		bActive = false;
		Master->StorageUI = NULL;
	}
	else
	{
		Master->StorageUI = CreateWidget(Cast<APlayerController>(Master->Controller), UIClass);
		Master->StorageUI->AddToViewport();
		bActive = true;
	}
	Master->EnableInput(Cast<APlayerController>(Master->Controller));

}