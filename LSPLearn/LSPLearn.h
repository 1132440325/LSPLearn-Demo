
// LSPLearn.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CLSPLearnApp: 
// �йش����ʵ�֣������ LSPLearn.cpp
//

class CLSPLearnApp : public CWinApp
{
public:
	CLSPLearnApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CLSPLearnApp theApp;