
// STISPLoader.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CSTISPLoaderApp: 
// �йش����ʵ�֣������ STISPLoader.cpp
//

class CSTISPLoaderApp : public CWinApp
{
public:
	CSTISPLoaderApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CSTISPLoaderApp theApp;