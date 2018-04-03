#pragma once

#include <afxdb.h>
#include "zsCommon.h"

class CBusiness;

class  CZsBindingReader {
public:
	CZsBindingReader(CBusiness * pBusiness);
	~CZsBindingReader();

	int Init();

	// ел╣Ц
	int Inventory(TagItem * pId);

	enum BINDING_STATUS {
		STATUS_CLOSE = 0,
		STATUS_OPEN,
	};

	BINDING_STATUS GetStatus();

private:
	void Clear();

	BINDING_STATUS      m_eStatus;

	CBusiness  *        m_pBusiness;
};

