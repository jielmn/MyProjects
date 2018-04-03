#pragma once

#include "LmnContainer.h"
#include "LmnConfig.h"
#include "LmnLog.h"
#include "LmnThread.h"

#define  DATABASE_STATUS_LABEL_ID              "lblDatabaseStatus"
#define  BINDING_READER_STATUS_LABEL_ID        "lblBindingReaderStatus"
#define  SYNC_READER_STATUS_LABEL_ID           "lblSyncReaderStatus"
#define  TABS_ID                       "switch"
#define  SUB_TABS_ID                   "switch_data"
#define  TEMPERATURE_DATA_CONTROL_ID   "TemperatureData"
#define  PATIENTS_CONTROL_ID           "Patients"
#define  NURSES_CONTROL_ID             "Nurses"
#define  SYNCHRONIZE_CONTROL_ID        "Synchronise"
#define  TEMPERATURE_DATA_OPTION_ID    "optTemperatureData"
#define  PATIENTS_OPTION_ID            "optPatients"
#define  NURSES_OPTION_ID              "optNurses"
#define  SYNCHRONIZE_OPTION_ID         "optSynchronise"
#define  PURE_DATA_CONTROL_ID          "PureData"
#define  CURVE_CONTROL_ID              "TemperatureCurve"
#define  PURE_DATA_OPTION_ID           "optPureData"
#define  CURVE_OPTION_ID               "optCurveData"
#define  ADD_PATIENT_BUTTON_ID         "btnAddPatient"
#define  MOD_PATIENT_BUTTON_ID         "btnModifyPatient"
#define  DEL_PATIENT_BUTTON_ID         "btnDelPatient"
#define  IMPORT_PATIENT_BUTTON_ID      "import_excel_patient"
#define  BINDING_PATIENT_BUTTON_ID      "btnBinding_patient"
#define  PATIENT_ID_EDIT_ID             "edPatientId"
#define  PATIENT_NAME_EDIT_ID           "edPatientName"
#define  MALE_OPTION_ID                 "btnMale"
#define  FEMALE_OPTION_ID               "btnFemale"
#define  BED_NO_EDIT_ID                 "edBedNo"
#define  TAGS_LAYOUT_ID                 "layoutTags"
#define  PATIENT_OK_BUTTON_ID           "btnPatientOk"
#define  IN_HOS_OPTION_ID               "btnInhos"
#define  OUT_HOS_OPTION_ID              "btnOutHos"
#define  PATIENTS_LIST_ID               "patients_list"

#define  TEMPERATURE_DATA_FILE         "TemperatureData.xml"
#define  PATIENTS_FILE                 "Patients.xml"
#define  NURSES_FILE                   "Nurses.xml"
#define  SYNCHRONIZE_FILE              "Synchronise.xml"
#define  PURE_DATA_FILE                "PureData.xml"
#define  CURVE_FILE                    "TemperatureCurve.xml"


#define  SET_CONTROL_TEXT(ctrl,text)      do { if ( ctrl ) { ctrl->SetText(text); } } while( 0 )
#define  GET_CONTROL_TEXT(ctrl)           ( (ctrl == 0) ? "" : ctrl->GetText() )
#define  SET_CONTROL_ENABLED( ctrl, e )   do { if ( ctrl ) { ctrl->SetEnabled(e); } } while( 0 )
#define  SET_CONTROL_COLOR( ctrl, e )     do { if ( ctrl ) { ctrl->SetTextColor(e); } } while( 0 )
#define  SET_CONTROL_VISIBLE( ctrl, e )   do { if ( ctrl ) { ctrl->SetVisible(e); } } while( 0 )


// config配置字符串常量
#define ODBC_TYPE                  "odbc type"
#define ODBC_STRING_ORACLE         "odbc string oracle"
#define ODBC_STRING_MYSQL          "odbc string mysql"

// db reconnect 时间
#define  RECONNECT_DB_TIME         10000
#define  RECONNECT_READER_TIME     10000

// thread message
#define MSG_RECONNECT_DB           1
#define MSG_INVENTORY              2
#define MSG_ADD_PATIENT            3

// windows 自定义消息
#define UM_SHOW_DB_STATUS                      (WM_USER+1)
#define UM_SHOW_BINDING_READER_STATUS          (WM_USER+2)
#define UM_ADD_PATIENT_RET                     (WM_USER+3)

// 错误码
#define ZS_ERR_NO_MEMORY                     10001
#define ZS_ERR_FAILED_TO_LOAD_FUNCTION_DLL   10002
#define ZS_ERR_NOT_FOUND_TAG                            10003
#define ZS_ERR_BINDING_READER_FAILED_TO_INVENTORY       10004
#define ZS_ERR_BINDING_READER_FAILED_TOO_MANY_CARDS     10005
#define ZS_ERR_DB_CLOSE                                 10006
#define ZS_ERR_DB_ERROR                                 10007
#define ZS_ERR_DB_NOT_UNIQUE                            10008

#define  DB_STATUS_OK_TEXT             "数据库连接OK"
#define  DB_STATUS_CLOSE_TEXT          "数据库连接失败"
#define  BINDING_READER_STATUS_OK_TEXT             "绑定读卡器连接OK"
#define  BINDING_READER_STATUS_CLOSE_TEXT          "绑定读卡器连接失败"

#define  COLOR_OK                                  0xFFFFFFFF
#define  COLOR_ERROR                               0xFFCAF100

#define  CAPTION_PATIENT_MSG_BOX                   "添加/修改病人信息"

#define  MAX_TAG_ID_LENGTH              32
//  tag
typedef struct tagTagItem {
	BYTE     abyUid[MAX_TAG_ID_LENGTH];
	DWORD    dwUidLen;
}TagItem;


#define  MAX_PATIENT_ID_LENGTH              32
#define  MAX_PATIENT_NAME_LENGTH            32
#define  MAX_BED_NO_LENGTH                  32
// 病人
typedef struct tagPatientInfo {
	DWORD    dwId;
	char     szId[MAX_PATIENT_ID_LENGTH];
	char     szName[MAX_PATIENT_NAME_LENGTH];
	char     szBedNo[MAX_BED_NO_LENGTH];
	BOOL     bFemale;
	BOOL     bOutHos;
}PatientInfo;


class CAddPatientParam : public LmnToolkits::MessageData {
public:
	CAddPatientParam(const PatientInfo * pPatient, HWND hWnd) {
		if (pPatient)
			memcpy(&m_patient, pPatient, sizeof(PatientInfo));
		else
			memset(&m_patient, 0, sizeof(PatientInfo));

		m_hWnd = hWnd;
	}
	~CAddPatientParam() {}

	PatientInfo   m_patient;
	HWND          m_hWnd;
};


extern ILog    * g_log;
extern IConfig * g_cfg;
extern HWND    g_hWnd;
extern LmnToolkits::Thread *  g_thrd_db;
extern LmnToolkits::Thread *  g_thrd_reader;
extern LmnToolkits::Thread *  g_thrd_timer;

extern char * MyEncrypt(const void * pSrc, DWORD dwSrcSize, char * dest, DWORD dwDestSize);
extern int    MyDecrypt(const char * szSrc, void * pDest, DWORD & dwDestSize);
extern const char * GetErrDescription(int e);
extern const char * GetGender(BOOL bFemale);
extern const char * GetInHosStatus(BOOL bOutHos);