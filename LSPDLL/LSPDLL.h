// LSPDLL.h : LSPDLL DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CLSPDLLApp
// �йش���ʵ�ֵ���Ϣ������� LSPDLL.cpp
//

class CLSPDLLApp : public CWinApp
{
public:
	CLSPDLLApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
