// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once 
#include "GameFramework/HUD.h"
#include "RefugeeSimulatorHUD.generated.h"

UCLASS()
class ARefugeeSimulatorHUD : public AHUD
{
	GENERATED_BODY()

public:
	ARefugeeSimulatorHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:

};

