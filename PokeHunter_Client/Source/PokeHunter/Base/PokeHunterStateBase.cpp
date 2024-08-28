// Fill out your copyright notice in the Description page of Project Settings.


#include "PokeHunterStateBase.h"
#include "Net/UnrealNetwork.h"

void APokeHunterStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APokeHunterStateBase, LatestEvent);
	DOREPLIFETIME(APokeHunterStateBase, WinningTeam);
}
