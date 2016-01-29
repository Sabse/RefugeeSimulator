// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "UnrealSerial.h"
#include "EKGCommunicationActor.generated.h"

/**
 * 
 */
UCLASS()
class UEKGCommunicationActor : public UBlueprintFunctionLibrary
{

	GENERATED_BODY()

public: 

	UFUNCTION(BlueprintCallable, Category = "USB Communication")
		static int32 ReadByte();

	UFUNCTION(BlueprintCallable, Category = "USB Communication")
		static void SendByte(int32 byte);
};
