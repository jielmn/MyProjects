#include <time.h>
#include "common.h"

CGlobalData  g_data;
std::map<int, DWORD>        g_color;

CChannel::CChannel(DWORD dwArgb) : m_pen(Gdiplus::Color(dwArgb)), m_brush(Gdiplus::Color(dwArgb)) {

}

CChannel::~CChannel() {

}