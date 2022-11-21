// Fill out your copyright notice in the Description page of Project Settings.

#include "Hunter.h"

#include <mftransform.h>
#include <vector>

#include "InventoryComponent.h"
#include "HunterAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components\SphereComponent.h"
#include "Components\CapsuleComponent.h"
#include "Components/TimelineComponent.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "PokeHunter/Npc/Npc.h"
#include "PokeHunter/Partner/Partner.h"
#include "PokeHunter/Item/Item.h"
#include "PokeHunter/Item/ItemData.h"
#include "PokeHunter/Base/InteractActor.h"



// Sets default values
AHunter::AHunter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//  Mesh
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_HUNTER(TEXT("/Game/Hunter/Asset/YBot/Y_Bot.Y_Bot"));
	if (SK_HUNTER.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SK_HUNTER.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -90.0f), FRotator(0.0f, -90.0f, 0.0f));
	}

	// Animation
	static ConstructorHelpers::FClassFinder<UAnimInstance> ANIM_HUNTER(TEXT("/Game/Hunter/Blueprint/BP_HunterAnimInstance"));
	if (ANIM_HUNTER.Succeeded()) 
	{
		GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
		GetMesh()->SetAnimInstanceClass(ANIM_HUNTER.Class);
	}

	//Timeline
	static ConstructorHelpers::FObjectFinder<UCurveFloat>DIVE_CURVE(TEXT("/Game/Hunter/Blueprint/DiveCurve.DiveCurve"));
	if (DIVE_CURVE.Succeeded())
	{
		DiveCurve = DIVE_CURVE.Object;
	}



	//Camera
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetOffset = FVector(0, 0, GetDefaultHalfHeight());
	CameraBoom->TargetArmLength = 500.f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	//Collision
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AHunter::OnOverlapBegin);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &AHunter::OnOverlapEnd);

	//UI
	static ConstructorHelpers::FClassFinder<UUserWidget>TempInvenClass(TEXT("/Game/UI/WBP_InventoryList"));
	if (TempInvenClass.Succeeded())
	{
		InventoryUIClass = TempInvenClass.Class;
	}
	
	static ConstructorHelpers::FClassFinder<UUserWidget>TempMainClass(TEXT("/Game/UI/WBP_MainUI"));
	if (TempMainClass.Succeeded())
	{
		MainUIClass = TempMainClass.Class;
	}

	//Using Controller Rotation
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	// Character Rotation Movement
	GetCharacterMovement()->bOrientRotationToMovement = true;

	//Inventory
	Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	Inventory->capacity = 20;
	Inventory->Hunter = this;

	//Set Quickslot Default
	for (int i = 0; i < 10; ++i)
	{
		QuickSlotArray.AddDefaulted();
	}

}

// Called when the game starts or when spawned
void AHunter::BeginPlay()
{
	Super::BeginPlay();

	//Delegate
	FIKeyDelegate.AddDynamic(this, &AHunter::OpenInventory);
	FMouseWheelDelegate.AddDynamic(this, &AHunter::ChangeQuickslot);

	//카메라 래깅 관련
	// Controller->bFindCameraComponentWhenViewTarget = true;

	//UI
	MainUI = CreateWidget(GetWorld(), MainUIClass, TEXT("MainUI"));
	MainUI->AddToViewport();

	//Timeline
	DiveInterpCallback.BindUFunction(this, FName("DiveInterpReturn"));
	if (DiveCurve)
	{
		DiveTimeline.AddInterpFloat(DiveCurve, DiveInterpCallback);
		DiveTimeline.SetTimelineLength(1.63f);
	}

}

// Called every frame
void AHunter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DiveTimeline.TickTimeline(DeltaTime);
}

void AHunter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	auto AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->OnMontageEnded.AddDynamic(this, &AHunter::OnCombatMontageEnded);
}

void AHunter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
}

// Called to bind functionality to input
void AHunter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &AHunter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AHunter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &AHunter::AddControllerYawInput);		// X
	PlayerInputComponent->BindAxis("LookUp", this, &AHunter::AddControllerPitchInput);	// Y

	PlayerInputComponent->BindAxis("MouseWheel", this, &AHunter::WheelInput);

	PlayerInputComponent->BindAction("SpaceBar", IE_Pressed, this, &AHunter::SpaceDown);
	PlayerInputComponent->BindAction("LShift", IE_Pressed, this, &AHunter::LShiftDown);
	PlayerInputComponent->BindAction("LShift", IE_Released, this, &AHunter::LShiftUp);
	PlayerInputComponent->BindAction("LMB", IE_Pressed, this, &AHunter::LMBDown);
	PlayerInputComponent->BindAction("RMB", IE_Pressed, this, &AHunter::RMBDown);
	PlayerInputComponent->BindAction("RMB", IE_Released, this, &AHunter::RMBUp);
	PlayerInputComponent->BindAction("I_Key", IE_Pressed, this, &AHunter::OpenInventory);
}
void AHunter::SpaceDown()
{
	if(bDiving) return;
	
	if(!bZoom)
	{
		bDiving = 1;
		auto AnimInstance = Cast<UHunterAnimInstance>(GetMesh()->GetAnimInstance());
		if(AnimInstance) AnimInstance->PlayCombatMontage();
		FVector Speed = GetVelocity();
		FVector XYspeed = FVector(Speed.X, Speed.Y, 0.f);
		if (XYspeed.Size() <= 300.f) LastSpeed = 300.0f;
		else 
		{
			LastSpeed = XYspeed.Size();
		}
		DiveTimeline.PlayFromStart();
	}
}

void AHunter::MoveForward(float Val)
{
	if(bDiving) return;
	
	if (Val != 0.0f && !bDiving)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		//���� ����
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Val);
	}
}

void AHunter::MoveRight(float Val)
{
	if(bDiving) return;
	
	if (Val != 0.0f && !bDiving)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		//���� ����
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Val);
	}
}

void AHunter::Turn(float NewAxisValue)
{
	AddControllerYawInput(NewAxisValue);
}

void AHunter::LookUp(float NewAxisValue)
{
	AddControllerPitchInput(NewAxisValue);
}

void AHunter::LShiftDown()
{
	if(bDiving) return;
	
	if(!bZoom)
	{
		bRunning = 1;
		GetCharacterMovement()->MaxWalkSpeed = 1000.f;
	}
}

void AHunter::LShiftUp()
{
	
}

void AHunter::LMBDown()
{
	if(bDiving) return;
	
	if (bZoom)
	{
		//����
	}
	else 
	{
		//������ ���
		if (QuickSlotArray[CurQuickKey] != NULL)
		{
			//Use Item Update need
			//GetWorld()->SpawnActor<AItem>(QuickSlotArray[CurQuickKey]->ItemClass, GetActorLocation(), GetControlRotation());

			/*QuickSlotArray[CurQuickKey]->--;
			if (QuickSlotArray[CurQuickKey]->ItemCount == 0)
			{
				Inventory->ItemArray[QuickSlotArray[CurQuickKey]->ItemIndex] = NULL;
				QuickSlotArray[CurQuickKey]->ConditionalBeginDestroy();
				QuickSlotArray[CurQuickKey] = NULL;
			}*/

		}
	}
}

void AHunter::RMBDown()
{
	if(bDiving) return;
	
	if (InteractingActor)
	{
		InteractingActor->Interact_Implementation(this);
		return;
	}

	CameraBoom->TargetArmLength = 100;
	bUseControllerRotationYaw = true;
	bUseControllerRotationPitch = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	bZoom = true;

	GetCharacterMovement()->MaxWalkSpeed = 300.f;
}

void AHunter::RMBUp()
{
	if(bZoom)
	{
		CameraBoom->TargetArmLength = 300;
		bUseControllerRotationYaw = false;
		bUseControllerRotationPitch = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
		bZoom = false;

		SetActorRelativeRotation(FRotator(0, GetControlRotation().Yaw, GetControlRotation().Roll));
		Cast<UCharacterMovementComponent>(GetCharacterMovement())->MaxWalkSpeed = 600.0f;
	}
}

void AHunter::WheelInput(float Val)
{
	if (Val != 0.0f)
	{
		ChangeQuickslot(Val);
	}
}

void AHunter::ChangeQuickslot(float Val)
{
	CurQuickKey += int(Val);
	if (CurQuickKey < 0) CurQuickKey += 10;
	else if (CurQuickKey > 9) CurQuickKey -= 10;
	if (FMouseWheelDelegate.IsBound()) FMouseWheelDelegate.Broadcast(Val);
}

void AHunter::SetQuickslot(class UItemData* TargetData, int Key)
{
	QuickSlotArray[Key] = TargetData;
}

void AHunter::OpenInventory()
{
	if (InventoryUI == nullptr)
	{


		//InventoryUI = CreateWidget<UUserWidget>(GetWorld(), InventoryUIClass, TEXT("Inventory"));
		//InventoryUI = MainUI->CreateWidgetInstance(*GetWorld(), InventoryUIClass, TEXT("Inventory"));
		//InventoryUI->AddToViewport();
		//InventoryUI->Visibility = ESlateVisibility::Visible;
	}
	
	else 
	{
		if (InventoryUI->Visibility == ESlateVisibility::Visible)
		{
			InventoryUI->Visibility = ESlateVisibility::Hidden;
		}
		else if(InventoryUI->Visibility == ESlateVisibility::Hidden)
		{
			InventoryUI->Visibility = ESlateVisibility::Visible;
		}
	}
}

void AHunter::OnCombatMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if(bDiving) bDiving = false;
}


void AHunter::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OverlappedComp == GetCapsuleComponent()) UE_LOG(LogTemp, Warning, TEXT("OverlapBegin"));
	if (InteractingActor == nullptr)
	{
		if (OtherActor->GetClass()->ImplementsInterface(UInteractInterface::StaticClass()))
		{
			InteractingActor = Cast<AInteractActor>(OtherActor);
		}
	}
}

void AHunter::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->GetClass()->ImplementsInterface(UInteractInterface::StaticClass()))
	{
		if (InteractingActor == Cast<AInteractActor>(OtherActor))
		{
			UE_LOG(LogTemp, Warning, TEXT("OverlapEnd"));
			InteractingActor = NULL;
		}
		else return;
	}

	if (InteractingActor == nullptr)
	{
		TArray<AActor*> OverlappedActors;
		GetCapsuleComponent()->GetOverlappingActors(OverlappedActors);
		for (int32 i = 0; i < OverlappedActors.Num(); ++i)
		{
			if (OverlappedActors[i]->GetClass()->ImplementsInterface(UInteractInterface::StaticClass()))
			{
				InteractingActor = Cast<AInteractActor>(OverlappedActors[i]);
				break;
			}
		}
	}
}

void AHunter::DiveInterpReturn(float Value)
{
	auto HunterMovement = GetCharacterMovement();
	FVector CurLocation = GetActorLocation();
	FVector GoalLocation = GetActorLocation() + GetActorForwardVector() * LastSpeed;
	
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	ActorLineTraceSingle(HitResult, CurLocation, GoalLocation, ECC_WorldDynamic, QueryParams);
	if (HitResult.bBlockingHit)
	{
		GoalLocation = HitResult.Location;
		UE_LOG(LogTemp, Warning, TEXT("Collision"));
	}
	SetActorLocation(FMath::VInterpTo(CurLocation, GoalLocation, GetWorld()->GetDeltaSeconds(), 1.0f));
}
