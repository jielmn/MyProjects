#pragma once

#include "common.h"
#include "LmnSerialPort.h"

class CBusiness;

class  CLaunch : public CLmnSerialPort {
public:
	CLaunch( );
	~CLaunch();

	// ����
	int  Reconnect();

	// Ӳ���Ķ������״̬
	int  CheckStatus();
};