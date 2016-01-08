// Fill out your copyright notice in the Description page of Project Settings.

#include "RefugeeSimulator.h"
#include "RefugeeSimulatorProtagonist.h"
#include "GameFramework/InputSettings.h"
#include "Engine.h"
#include <fstream>
#include <string>

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

// Sets default values
ARefugeeSimulatorProtagonist::ARefugeeSimulatorProtagonist()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	ProtagonistCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	ProtagonistCameraComponent->AttachParent = GetCapsuleComponent();
	ProtagonistCameraComponent->RelativeLocation = FVector(0, 0, 64.f); // Position the camera
	ProtagonistCameraComponent->bUsePawnControlRotation = true;

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P are set in the
	// derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

//////////////////////////////////////////////////////////////////////////
// Input

void ARefugeeSimulatorProtagonist::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// set up gameplay key bindings
	check(InputComponent);

	InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	InputComponent->BindAction("PrintLogData", IE_Pressed, this, &ARefugeeSimulatorProtagonist::printToLogfile);

	InputComponent->BindAxis("MoveForward", this, &ARefugeeSimulatorProtagonist::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ARefugeeSimulatorProtagonist::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &ARefugeeSimulatorProtagonist::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &ARefugeeSimulatorProtagonist::LookUpAtRate);
}

void ARefugeeSimulatorProtagonist::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void ARefugeeSimulatorProtagonist::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	TouchItem.bIsPressed = false;
}

void ARefugeeSimulatorProtagonist::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if ((TouchItem.bIsPressed == true) && (TouchItem.FingerIndex == FingerIndex))
	{
		if (TouchItem.bIsPressed)
		{
			if (GetWorld() != nullptr)
			{
				UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
				if (ViewportClient != nullptr)
				{
					FVector MoveDelta = Location - TouchItem.Location;
					FVector2D ScreenSize;
					ViewportClient->GetViewportSize(ScreenSize);
					FVector2D ScaledDelta = FVector2D(MoveDelta.X, MoveDelta.Y) / ScreenSize;
					if (ScaledDelta.X != 0.0f)
					{
						TouchItem.bMoved = true;
						float Value = ScaledDelta.X * BaseTurnRate;
						AddControllerYawInput(Value);
					}
					if (ScaledDelta.Y != 0.0f)
					{
						TouchItem.bMoved = true;
						float Value = ScaledDelta.Y* BaseTurnRate;
						AddControllerPitchInput(Value);
					}
					TouchItem.Location = Location;
				}
				TouchItem.Location = Location;
			}
		}
	}
}

void ARefugeeSimulatorProtagonist::MoveForward(float Value)
{
	if (Value != 0.0f && movementEnabled)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ARefugeeSimulatorProtagonist::MoveRight(float Value)
{
	if (Value != 0.0f && movementEnabled)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ARefugeeSimulatorProtagonist::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ARefugeeSimulatorProtagonist::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool ARefugeeSimulatorProtagonist::EnableTouchscreenMovement(class UInputComponent* InputComponent)
{
	bool bResult = false;
	if (FPlatformMisc::GetUseVirtualJoysticks() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		bResult = true;
		InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ARefugeeSimulatorProtagonist::BeginTouch);
		InputComponent->BindTouch(EInputEvent::IE_Released, this, &ARefugeeSimulatorProtagonist::EndTouch);
		InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &ARefugeeSimulatorProtagonist::TouchUpdate);
	}
	return bResult;
}


void ARefugeeSimulatorProtagonist::logEvent(float runtime, FString eventName, FString action)
{
	int minutes = timeCalculator(int(runtime), 0);
	int seconds = int(runtime) - (minutes * 60);
	FString minutesString = FString::FromInt(minutes);
	FString secondsString = FString::FromInt(seconds);
	FString logMessage = TEXT("[" + minutesString + ": " + secondsString + "]" + " " + eventName + " " + action);

	//add string to string array
	logData.Add(logMessage);

}

void ARefugeeSimulatorProtagonist::printToLogfile()
{
	FString* logMPtr = logData.GetData();
	FString fileName = TEXT("LogFile");
	fileName += TEXT(".txt");
	FString saveFile;
	saveFile += "/RSLogFiles/";
	saveFile += fileName;

	FString eventLogs;
	for (int i = 0; i < logData.Num(); i++)
	{
		eventLogs += logMPtr[i];
		eventLogs += LINE_TERMINATOR;
	}

	FFileHelper::SaveStringToFile(eventLogs, *saveFile);
}

int ARefugeeSimulatorProtagonist::timeCalculator(int seconds, int minutes) {
	return seconds / 60;
}

