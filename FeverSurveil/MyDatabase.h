#pragma once

class  CMyDatabase {
public:
	CMyDatabase();
	~CMyDatabase();

	DbStatus   GetStatus() const;
	void       ReconnectDb();
	int        UpdateFeverData(const  LmnToolkits::MessageData * pParam);

	void       Close();
private:
	MYSQL      m_mysql;
	DbStatus   m_eStatus;
	int        m_content_board_index;

	void       LogError();
};