#pragma once

#include "common.h"
#include "LmnSerialPort.h"

class CBusiness;

class  CLaunch : public CLmnSerialPort {
public:
	CLaunch( );
	~CLaunch();

	// 重连
	int  Reconnect();

	// 硬件改动，检查状态
	int  CheckStatus();
};