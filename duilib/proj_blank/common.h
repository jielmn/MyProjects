#pragma once

#include "LmnCommon.h"
#include "LmnConfig.h"
#include "LmnLog.h"
#include "LmnThread.h"
#include "LmnString.h"

#define   LOG_FILE_NAME           "blank.log"
#define   CONFIG_FILE_NAME        "blank.cfg"

extern ILog    * g_log;
extern IConfig * g_cfg;
extern LmnToolkits::Thread *  g_thrd_db;