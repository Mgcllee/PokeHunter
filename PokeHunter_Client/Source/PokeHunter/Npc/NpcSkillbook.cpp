// Fill out your copyright notice in the Description page of Project Settings.


#include "NpcSkillbook.h"
#include "Blueprint/UserWidget.h"

ANpcSkillbook::ANpcSkillbook()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ANpcSkillbook::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickEnabled(0);
}

// Called every frame
void ANpcSkillbook::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ANpcSkillbook::Interact_Implementation(AHunter* Hunter)
{
	ANpc::Interact_Implementation(Hunter);
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ANpcSkillbook::OpenUI, 1.0f, false, 1.0f);

}
