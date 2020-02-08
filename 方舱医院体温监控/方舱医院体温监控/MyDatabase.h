#pragma once

#include "sqlite3.h"
#include "common.h"

class CMySqliteDatabase  {
public:
	CMySqliteDatabase();
	int InitDb();
	int DeinitDb();

	void  SaveSurTemp(CSaveSurTempParam * pParam);
	// 保存手持温度数据
	void  SaveHandTemp(CSaveHandTempParam * pParam);
	// 根据TagID查询温度数据
	void  QueryTempByTag(const char * szTagId, std::vector<TempItem*> & vRet);
	// 保存注释
	void  SaveRemark(const CSaveRemarkParam * pParam);
	// 根据TagID查询Tag的病人姓名
	void  QueryTagPNameByTagId(const char * szTagId, char * szPName, DWORD  dwPNameLen);
	// 保存最后的术中tag id
	void SaveLastSurTagId(const CSaveLastSurTagId * pParam);
	// 获取最后一次的术中TagId
	void GetAllLastSurTags(std::vector<LastSurTagItem *> & vLastSurTags);
	// 获取所有的手持Tag温度数据
	void GetAllHandTagTempData(std::vector<HandTagResult *> & vHandTagRet);
	// 保存Tag和窗格绑定
	void TagBindingGrid(const CBindingTagGrid * pParam, std::string & old_tagid, int & old_grid_index);
	// 删除格子的手持Tag绑定
	void RemoveGridBinding(const CRemoveGridBindingParam * pParam);
	// 保存Tag的patient name
	void SaveTagPName(const CSaveTagPNameParam * pParam);
	// 查询tag绑定的grid index
	int  QueryBindingIndexByTag(const char * szTagId);
	// 查询PatientInfo
	void QueryPatientInfo(const CQueryPatientInfoParam * pParam, PatientInfo * pRet, 
		std::vector<PatientEvent * > & vEvents);
	// 保存info
	void SavePatientInfo(const CSavePatientInfoParam * pParam);
	// 保存patient events
	void SavePatientEvents(const CSavePatientEventsParam * pParam);
	// 查询PatientData
	void QueryPatientData( const CQueryPatientDataParam * pParam, PatientData * pData, 
		                   DWORD dwSize, std::vector<TempItem *> & vTemp );
	// 保存data
	void SavePatientData(const CSavePatientDataParam * pParam);
	// 删除tag
	void DelTag(const CDelTag * pParam);
	// 是否出院
	BOOL IsOutHospital(const char * szTagId);
	// 查询住院信息
	void QueryInHospital(const CQueryInHospital * pParam, std::vector<InHospitalItem * > & vRet);
	// 查询出院信息
	void QueryOutHospital(const CQueryOutHospital * pParam, std::vector<OutHospitalItem * > & vRet);

private:
	sqlite3 *                         m_db;

private:
	// szTableName: 要创建的表名
	// szSql: create table (...)，括号里面的内容
	BOOL  CreateTable(const char * szTableName, const char * szSql);
	// 删除过时的温度数据，Tag数据
	void  PruneOldData();
	// 拼接Where子句
	void  ConcatWhereClause(CDuiString & strClause, const CDuiString & strItem);
	// 查询事件信息
	void  QueryEventsByTag(const char * szTagId, InHospitalItem * pRet);
	void  QueryEventsByTag(const char * szTagId, OutHospitalItem * pRet);
	// 更新config表里的版本号
	void  UpdateConfigVersion(BOOL bCreated);
	int   GetVersion();
	// 转换tag名称
	void  ConvertTagNames();

public:
	// 根据TagID查询绑定的grid index(Return: 0 notfound, > 0 found)
	int  QueryBindingGridIndexByTagId(const char * szTagId);

	// 方舱获取所有床位号
	void  GetAllCubeBeds(std::vector<CubeItem*> & vRet);
};