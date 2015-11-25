// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "RefugeeSimulator.h"
#include "RefugeeSimulatorGameMode.h"
#include "RefugeeSimulatorHUD.h"
#include "RefugeeSimulatorCharacter.h"

ARefugeeSimulatorGameMode::ARefugeeSimulatorGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ARefugeeSimulatorHUD::StaticClass();
}
