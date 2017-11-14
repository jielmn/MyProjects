#pragma once

class CMyReader {
public:
	CMyReader();
	~CMyReader();

	void Open();
	void Inventory();
	int  GetStatus() const;

private:
	int                 m_nStatus;
	RFID_READER_HANDLE  m_hr_reader;
	BOOL                m_bNewAI;
	BOOL                m_enISO15693;
	BOOL 	            m_enAFI;
	DWORD	            m_afi;
	BOOL	            m_enISO14443a;
	BOOL                m_enISO18000p3m3;
};