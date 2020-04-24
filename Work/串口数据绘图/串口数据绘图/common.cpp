#include <time.h>
#include "common.h"

CGlobalData  g_data;


CChannel::CChannel(DWORD dwArgb) : m_pen(Gdiplus::Color(dwArgb)) {

}

CChannel::~CChannel() {

}