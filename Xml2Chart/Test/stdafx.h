
// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // �� Windows ͷ���ų�����ʹ�õ�����
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��

// �ر� MFC ��ĳЩ�����������ɷ��ĺ��Եľ�����Ϣ������
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC ��������ͱ�׼���
#include <afxext.h>         // MFC ��չ


#include <afxdisp.h>        // MFC �Զ�����



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC �� Internet Explorer 4 �����ؼ���֧��
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC �� Windows �����ؼ���֧��
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // �������Ϳؼ����� MFC ֧��









#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


#include "Xml2Chart.h"

#define XML_CHART_WIDTH              760
#define XML_CHART_HEIGHT             1044

#define  MAX_TAG_ID_LENGTH                  20
#define  MAX_TAG_PNAME_LENGTH               20

#define  MAX_OUTPATIENT_NO_LENGTH           20
#define  MAX_HOSPITAL_ADMISSION_NO_LENGTH   20
#define  MAX_MEDICAL_DEPARTMENT_LENGTH      20
#define  MAX_WARD_LENGTH                    20
#define  MAX_BED_NO_LENGTH                  20

// ���˵Ļ�����Ϣ
typedef  struct  tagPatientInfo {
	char        m_szTagId[MAX_TAG_ID_LENGTH];
	char        m_szPName[MAX_TAG_PNAME_LENGTH];

	int         m_sex;
	int         m_age;
	char        m_szOutpatientNo[MAX_OUTPATIENT_NO_LENGTH];
	char        m_szHospitalAdmissionNo[MAX_HOSPITAL_ADMISSION_NO_LENGTH];
	time_t      m_in_hospital;
	char        m_szMedicalDepartment[MAX_MEDICAL_DEPARTMENT_LENGTH];
	char        m_szWard[MAX_WARD_LENGTH];
	char        m_szBedNo[MAX_BED_NO_LENGTH];
	time_t      m_surgery;
}PatientInfo;

#define  MAX_BLOOD_PRESSURE_LENGTH 20
#define  MAX_WEIGHT_LENGTH         20
#define  MAX_IRRITABILITY_LENGTH   20
// ���˵ķ���������
typedef  struct   tagPatientData {
	char        m_szTagId[MAX_TAG_ID_LENGTH];
	time_t      m_date;

	int         m_pulse[6];
	int         m_breath[6];
	int         m_defecate;
	int         m_urine;
	int         m_income;
	int         m_output;
	char        m_szBloodPressure[MAX_BLOOD_PRESSURE_LENGTH];
	char        m_szWeight[MAX_WEIGHT_LENGTH];
	char        m_szIrritability[MAX_IRRITABILITY_LENGTH];

	int         m_temp[6];
}PatientData;

void PrepareXmlChart(CXml2ChartFile & xmlChart, PatientInfo * pInfo,
	PatientData * pData, DWORD dwSize, time_t tFirstDay);

const char * GetSexStr(int nSex);
char * PreviewNum(char * buf, DWORD dwSize, int nNum);
void GetDateStr(char * year, DWORD d1, char * month, DWORD d2, char * day, DWORD d3, time_t t);
int GetPatientDataStartIndex(PatientData * pData, DWORD dwSize);
char * Date2String_md(char * szDest, DWORD dwDestSize, const time_t * t);
char * Date2String_md(char * szDest, DWORD dwDestSize, const time_t * t);
void PrintXmlChart(HDC hDC, CXml2ChartFile & xmlChart, int nOffsetX, int nOffsetY,
	PatientData * pData, DWORD dwDataSize);

