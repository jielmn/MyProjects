#pragma once
#include "common.h"
#include "LmnSerialPort.h"

class CVPort : public CLmnSerialPort {
public:
	CVPort() {}
	~CVPort() {}

	int  HandleData();

public:
	CDataBuf    m_recv;
};