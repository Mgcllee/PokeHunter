// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MainMenuGameMode.h"
#include "MainMenuHUD.h"

AMainMenuGameMode::AMainMenuGameMode()
{
	HUDClass = AMainMenuHUD::StaticClass();
}
