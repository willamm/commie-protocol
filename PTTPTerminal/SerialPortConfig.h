#pragma once

// By Will Murphy
inline BOOL OpenSerialPort(LPHANDLE hSerialPort, TCHAR* portName)
{
	//Checks the serial port
	*hSerialPort = CreateFile(portName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
	if (*hSerialPort == INVALID_HANDLE_VALUE)
	{
		// message box?
		return FALSE;
	}
	//configure the port as well

	return TRUE;
}

//By Will Murphy
inline BOOL ConfigurePortDcb(LPHANDLE hSerialPort, LPDCB lpdcb)
{
	// Set the DCB structure
	lpdcb->DCBlength = sizeof(DCB);
	lpdcb->BaudRate = CBR_9600;
	lpdcb->ByteSize = 8;
	lpdcb->Parity = NOPARITY;
	lpdcb->StopBits = ONESTOPBIT;
	lpdcb->EvtChar = 0x16;
	return TRUE;
	// Set the event masks for RTS/CTS
}


//By Will Murphy
inline BOOL ConfigurePortProps(LPHANDLE hSerialPort, LPCOMMPROP lpcp)
{
	lpcp->wPacketLength = DataFrame::FRAME_SIZE;
	return TRUE;
}

//By Will Murphy
inline BOOL ConfigurePortTimeouts(LPHANDLE hSerialPort, LPCOMMTIMEOUTS lpto)
{
	lpto->ReadIntervalTimeout = 2000; // temporary
	lpto->ReadTotalTimeoutMultiplier = 4; // temporary
	lpto->ReadTotalTimeoutConstant = 10; // temporary
	return TRUE;
}

//By Will Murphy
inline BOOL SetPortEvents()
{
	return TRUE;
}

