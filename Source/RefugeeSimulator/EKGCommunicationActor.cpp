// Fill out your copyright notice in the Description page of Project Settings.

#include "RefugeeSimulator.h"
#include "EKGCommunicationActor.h"
#include <iostream>
#include "Engine.h"

int32 UEKGCommunicationActor::ReadByte()
{
	UnrealSerial serial;

	if (serial.Open(3, 3900))
	{
		int32 limit = 256;
		char* lpBuffer = new char[500];
		int32 nByteRead = serial.ReadData(lpBuffer, limit);

		delete[]lpBuffer;
		serial.Close();

		return nByteRead;
	}
	return 0;
}

// 2/10, 4/12, 6/14, 
void UEKGCommunicationActor::SendByte(int32 byte)
{
	UnrealSerial serial;
	serial.Close();
	if (serial.Open(3, 3900))
	{		
		serial.WriteCommByte(byte);
		serial.WriteCommByte(0x00);
		
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("NOPE"));
	}
}
