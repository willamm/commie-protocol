#pragma once

class SerialPort
{
public:
	SerialPort();
	virtual ~SerialPort();

private:
	HANDLE mHandle;
	COMMCONFIG mCC;
	COMMPROP mCP;
	COMMTIMEOUTS mCTO;
	COMSTAT mCS;
	DCB mDCB;
};

