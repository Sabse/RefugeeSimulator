// Fill out your copyright notice in the Description page of Project Settings.

#include "RefugeeSimulator.h"
#include "UnrealSerial.h"

#include <stdio.h>
#include <tchar.h>

UnrealSerial::UnrealSerial()
{
	memset(&m_OverlappedRead, 0, sizeof(OVERLAPPED));
	memset(&m_OverlappedWrite, 0, sizeof(OVERLAPPED));
	m_hIDComDev = NULL;
	m_bOpened = false;
}

UnrealSerial::~UnrealSerial()
{
	Close();
}

bool UnrealSerial::Open(int nPort, int nBaud)
{
	if (m_bOpened)
	{
		return true;
	}
	WCHAR szPort[15];
	WCHAR szComParams[50];
	DCB dcb;

	swprintf_s(szPort, L"COM%d", nPort);
	m_hIDComDev = CreateFile(szPort, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
	if (m_hIDComDev == NULL) return(false);

	memset(&m_OverlappedRead, 0, sizeof(OVERLAPPED));
	memset(&m_OverlappedWrite, 0, sizeof(OVERLAPPED));

	COMMTIMEOUTS CommTimeOuts;
	CommTimeOuts.ReadIntervalTimeout = 0xFFFFFFFF;
	CommTimeOuts.ReadTotalTimeoutMultiplier = 0;
	CommTimeOuts.ReadTotalTimeoutConstant = 0;
	CommTimeOuts.WriteTotalTimeoutMultiplier = 0;
	CommTimeOuts.WriteTotalTimeoutConstant = 5000;
	SetCommTimeouts(m_hIDComDev, &CommTimeOuts);

	swprintf_s(szComParams, L"COM%d:%d,n,8,1", nPort, nBaud);

	m_OverlappedRead.hEvent = CreateEvent(NULL, true, false, NULL);
	m_OverlappedWrite.hEvent = CreateEvent(NULL, true, false, NULL);

	dcb.DCBlength = sizeof(DCB);
	GetCommState(m_hIDComDev, &dcb);
	dcb.BaudRate = nBaud;
	dcb.ByteSize = 8;
	unsigned char ucSet;
	ucSet = (unsigned char)((FC_RTSCTS & FC_DTRDSR) != 0);
	ucSet = (unsigned char)((FC_RTSCTS & FC_RTSCTS) != 0);
	ucSet = (unsigned char)((FC_RTSCTS & FC_XONXOFF) != 0);
	if (!SetCommState(m_hIDComDev, &dcb) ||
		!SetupComm(m_hIDComDev, 10000, 10000) ||
		m_OverlappedRead.hEvent == NULL ||
		m_OverlappedWrite.hEvent == NULL){
		unsigned long dwError = GetLastError();
		if (m_OverlappedRead.hEvent != NULL) CloseHandle(m_OverlappedRead.hEvent);
		if (m_OverlappedWrite.hEvent != NULL) CloseHandle(m_OverlappedWrite.hEvent);
		CloseHandle(m_hIDComDev);
		return(false);
	}

	m_bOpened = true;

	return(m_bOpened);

}

bool UnrealSerial::Close(void)
{
	if (!m_bOpened || m_hIDComDev == NULL) return(true);

	if (m_OverlappedRead.hEvent != NULL) CloseHandle(m_OverlappedRead.hEvent);
	if (m_OverlappedWrite.hEvent != NULL) CloseHandle(m_OverlappedWrite.hEvent);
	CloseHandle(m_hIDComDev);
	m_bOpened = false;
	m_hIDComDev = NULL;

	return(true);
}

int32 UnrealSerial::ReadData(void *buffer, int limit)
{
	if (!m_bOpened || m_hIDComDev == NULL) return(0);

	BOOL bReadStatus;
	unsigned long dwBytesRead, dwErrorFlags;
	COMSTAT ComStat;

	ClearCommError(m_hIDComDev, &dwErrorFlags, &ComStat);
	if (!ComStat.cbInQue) return(0);

	dwBytesRead = (unsigned long)ComStat.cbInQue;
	if (limit < (int)dwBytesRead) dwBytesRead = (unsigned long)limit;

	bReadStatus = ReadFile(m_hIDComDev, buffer, dwBytesRead, &dwBytesRead, &m_OverlappedRead);
	if (!bReadStatus){
		if (GetLastError() == ERROR_IO_PENDING){
			WaitForSingleObject(m_OverlappedRead.hEvent, 2000);
			return((int)dwBytesRead);
		}
		return(0);
	}

	return((int)dwBytesRead);
}

int32 UnrealSerial::SendData(char *buffer, int size)
{
	if (!m_bOpened || m_hIDComDev == NULL) return(0);

	unsigned long dwBytesWritten = 0;
	int i;
	for (i = 0; i<size; i++){
		WriteCommByte(buffer[i]);
		dwBytesWritten++;
	}

	return((int)dwBytesWritten);
}

int32 UnrealSerial::ReadDataWaiting(void)
{

	if (!m_bOpened || m_hIDComDev == NULL) return(0);

	unsigned long dwErrorFlags;
	COMSTAT ComStat;

	ClearCommError(m_hIDComDev, &dwErrorFlags, &ComStat);

	return((int)ComStat.cbInQue);
}

bool UnrealSerial::WriteCommByte(unsigned char ucByte)
{
	BOOL bWriteStat;
	unsigned long dwBytesWritten;

	bWriteStat = WriteFile(m_hIDComDev, (LPSTR)&ucByte, 1, &dwBytesWritten, &m_OverlappedWrite);
	if (!bWriteStat && (GetLastError() == ERROR_IO_PENDING)){
		if (WaitForSingleObject(m_OverlappedWrite.hEvent, 1000)) dwBytesWritten = 0;
		else{
			GetOverlappedResult(m_hIDComDev, &m_OverlappedWrite, &dwBytesWritten, false);
			m_OverlappedWrite.Offset += dwBytesWritten;
		}
	}

	return(true);

}
