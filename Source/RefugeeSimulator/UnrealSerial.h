// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#ifndef __SERIAL_H__
#define __SERIAL_H__

#define FC_DTRDSR       0x01
#define FC_RTSCTS       0x02
#define FC_XONXOFF      0x04
#define ASCII_BEL       0x07
#define ASCII_BS        0x08
#define ASCII_LF        0x0A
#define ASCII_CR        0x0D
#define ASCII_XON       0x11
#define ASCII_XOFF      0x13



/**
 * 
 */
class UnrealSerial
{


public:
	UnrealSerial();
	~UnrealSerial();

	bool Open(int nPort = 2, int nBaud = 9600);
	bool Close(void);

	int32  ReadData(void *, int);
	int32  SendData(char *, int);
	int32  ReadDataWaiting(void);

	bool IsOpened(void){ return(m_bOpened); }

	bool WriteCommByte(unsigned char);

protected:

	HANDLE m_hIDComDev;
	OVERLAPPED m_OverlappedRead, m_OverlappedWrite;
	bool m_bOpened;

};

#endif