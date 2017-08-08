// LSPDLL.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include "LSPDLL.h"

#include <Winsock2.h>
#include <Ws2spi.h>
#include <MyDebugTools.h>
#pragma comment(lib, "Ws2_32.lib")


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#ifndef __DEBUG_H__
#define __DEBUG_H__
#ifdef _DEBUG
#define ODS(szOut) \
{ \
	OutputDebugString(szOut); \
}
#define ODS1(szOut, var) \
{ \
	TCHAR sz[1024]; \
	_stprintf(sz, szOut, var); \
	OutputDebugString(sz); \
}
#else
#endif // _DEBUG
#endif // __DEBUG_H__

//
//TODO:  ����� DLL ����� MFC DLL �Ƕ�̬���ӵģ�
//		��Ӵ� DLL �������κε���
//		MFC �ĺ������뽫 AFX_MANAGE_STATE ����ӵ�
//		�ú�������ǰ�档
//
//		����: 
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// �˴�Ϊ��ͨ������
//		}
//
//		�˺������κ� MFC ����
//		������ÿ��������ʮ����Ҫ��  ����ζ��
//		��������Ϊ�����еĵ�һ�����
//		���֣������������ж������������
//		������Ϊ���ǵĹ��캯���������� MFC
//		DLL ���á�
//
//		�й�������ϸ��Ϣ��
//		����� MFC ����˵�� 33 �� 58��
//

// CLSPDLLApp

BEGIN_MESSAGE_MAP(CLSPDLLApp, CWinApp)
END_MESSAGE_MAP()


// CLSPDLLApp ����

CLSPDLLApp::CLSPDLLApp()
{
	// TODO:  �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CLSPDLLApp ����

CLSPDLLApp theApp;

#define MSGTYPE_PROCESS_NAME 0x1
#define MSGTYPE_PROXY_INFO 0x2
struct PipeMessage
{
	DWORD Type;
	CHAR Text[MAX_PATH + 1];
};

// CLSPDLLApp ��ʼ��
WSPUPCALLTABLE g_pUpCallTable; // �ϲ㺯���б����LSP �������Լ���α�������ʹ����������б�
WSPPROC_TABLE g_NextProcTable; // �²㺯���б�
TCHAR g_szCurrentApp[MAX_PATH]; // ��ǰ���ñ�DLL �ĳ��������
HANDLE m_hPipe;

CString SendPipeMessage(PipeMessage sPipe)
{
	WaitNamedPipe(L"\\\\.\\pipe\\XueLspPipe", //����ǿ�����ͨ�ţ�����Բ�㴦ָ���������˳������ڵ�������  
		NMPWAIT_WAIT_FOREVER);
	//�򿪿��õ������ܵ�������������˽��̽���ͨ��  
	HANDLE m_hPipe = CreateFile(L"\\\\.\\pipe\\XueLspPipe",
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == m_hPipe)
	{
		OutputDebugString(L"SendFail!");
		return NULL;
	}
	DWORD dwRead = 0;
	CStringA m_send(sPipe.Text);
	WriteFile(m_hPipe, &sPipe, sizeof(sPipe), &dwRead, NULL);
	WCHAR buffer[1024] = { 0 };
	OutputDebugString(L"wating return!");
	dwRead = 0;
	ReadFile(m_hPipe, buffer, 1024, &dwRead, NULL);
	OutputDebugString(L"get return!");
	DisconnectNamedPipe(m_hPipe);
	CloseHandle(m_hPipe);
	return CString((CHAR*)buffer);
}

BOOL CLSPDLLApp::InitInstance()
{
	CWinApp::InitInstance();
	::GetModuleFileName(NULL, g_szCurrentApp, MAX_PATH);
	//auto pid = _getpid();
	//CString str;
	//str.Format(L"����·��:%ws ����PID��%d", g_szCurrentApp, pid);
	//CString t_str = SendPipeMessage(str);
	//OutputDebugString(t_str);
	//MessageBox(0,t_str,0,0);
	return TRUE;
}

LPWSAPROTOCOL_INFOW GetProvider(LPINT lpnTotalProtocols)
{
	int nError;
	DWORD dwSize = 0;
	LPWSAPROTOCOL_INFOW pProtoInfo = NULL;
	// ȡ����Ҫ�Ļ���������
	if (::WSCEnumProtocols(NULL, pProtoInfo, &dwSize, &nError) == SOCKET_ERROR)
	{
		if (nError != WSAENOBUFS)
			return NULL;
	}
	// ���뻺�������ٴε���WSCEnumProtocols ����
	pProtoInfo = (LPWSAPROTOCOL_INFOW)::GlobalAlloc(GPTR, dwSize);
	*lpnTotalProtocols = ::WSCEnumProtocols(NULL, pProtoInfo, &dwSize, &nError);
	return pProtoInfo;
}
void FreeProvider(LPWSAPROTOCOL_INFOW pProtoInfo)
{
	::GlobalFree(pProtoInfo);
}



int WSPAPI WSPConnect(
	__in   SOCKET s,
	__in   const struct sockaddr *name,
	__in   int namelen,
	__in   LPWSABUF lpCallerData,
	__out  LPWSABUF lpCalleeData,
	__in   LPQOS lpSQOS,
	__in   LPQOS lpGQOS,
	__out  LPINT lpErrno
)
{
	SOCKADDR_IN *psi;
	psi = (SOCKADDR_IN*)name;
	ODS1(L"query send to... %s", g_szCurrentApp);
	CString target_address(inet_ntoa(psi->sin_addr));
	CString local_address(L"127.0.0.1");
	tssc(target_address);
	tssc(local_address);
	if (target_address == local_address) {
		return g_NextProcTable.lpWSPConnect(s, name, namelen, lpCallerData, lpCalleeData, lpSQOS, lpGQOS, lpErrno);
	}

	USES_CONVERSION;
	PipeMessage pmsg;
	pmsg.Type = MSGTYPE_PROCESS_NAME;
	strcpy(pmsg.Text, W2A(g_szCurrentApp));
	CString ret_str = SendPipeMessage(pmsg);
	OutputDebugStringW(ret_str);
	if (ret_str == L"1")
	{
		OutputDebugStringW(L"change ip");
		pmsg.Type = MSGTYPE_PROXY_INFO;
		memcpy(pmsg.Text, name, sizeof(SOCKADDR_IN));
		ret_str = SendPipeMessage(pmsg);
		SOCKADDR_IN si = { 0 };
		si.sin_family = AF_INET;
		si.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
		int Port = _wtoi(ret_str);
		si.sin_port = htons(Port);
		return g_NextProcTable.lpWSPConnect(s, (SOCKADDR*)&si, namelen, lpCallerData, lpCalleeData, lpSQOS, lpGQOS, lpErrno);
	}
	
	return g_NextProcTable.lpWSPConnect(s, name, namelen, lpCallerData, lpCalleeData, lpSQOS, lpGQOS, lpErrno);
	 
}

extern "C" int WSPAPI WSPStartup(
	WORD wVersionRequested,
	LPWSPDATA lpWSPData,
	LPWSAPROTOCOL_INFO lpProtocolInfo,
	WSPUPCALLTABLE UpcallTable,
	LPWSPPROC_TABLE lpProcTable
)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	// �˴�Ϊ��ͨ������
	int i = 0;
	ODS1(L" WSPStartup... %s \n", g_szCurrentApp);
	if (lpProtocolInfo->ProtocolChain.ChainLen <= 1)
	{
		return WSAEPROVIDERFAILEDINIT;
	}
	// �������ϵ��õĺ�����ָ�루�������ǲ�ʹ������
	g_pUpCallTable = UpcallTable;
	// ö��Э�飬�ҵ��²�Э���WSAPROTOCOL_INFOW�ṹ
	WSAPROTOCOL_INFOW NextProtocolInfo;
	int nTotalProtos;
	LPWSAPROTOCOL_INFOW pProtoInfo = GetProvider(&nTotalProtos);
	// �²����ID
	DWORD dwBaseEntryId = lpProtocolInfo->ProtocolChain.ChainEntries[1];
	for (int i = 0; i < nTotalProtos; i++)
	{
		if (pProtoInfo[i].dwCatalogEntryId == dwBaseEntryId)
		{
			memcpy(&NextProtocolInfo, &pProtoInfo[i], sizeof(NextProtocolInfo));
			break;
		}
	}
	if (i >= nTotalProtos)
	{
		ODS(L" WSPStartup: Can not find underlying protocol \n");
		return WSAEPROVIDERFAILEDINIT;
	}
	// �����²�Э���DLL
	int nError;
	TCHAR szBaseProviderDll[MAX_PATH];
	int nLen = MAX_PATH;
	// ȡ���²��ṩ��DLL ·��
	if (::WSCGetProviderPath(&NextProtocolInfo.ProviderId,
		szBaseProviderDll, &nLen, &nError) == SOCKET_ERROR)
	{
		ODS1(L" WSPStartup: WSCGetProviderPath() failed %d \n", nError);
		return WSAEPROVIDERFAILEDINIT;
	}
	if (!::ExpandEnvironmentStrings(szBaseProviderDll, szBaseProviderDll, MAX_PATH))
	{
		ODS1(L" WSPStartup: ExpandEnvironmentStrings() failed %d \n", ::GetLastError());
		return WSAEPROVIDERFAILEDINIT;
	}
	// �����²��ṩ��
	HMODULE hModule = ::LoadLibrary(szBaseProviderDll);
	if (hModule == NULL)
	{
		ODS1(L" WSPStartup: LoadLibrary() failed %d \n", ::GetLastError());
		return WSAEPROVIDERFAILEDINIT;
	}
	// �����²��ṩ�ߵ�WSPStartup ����
	LPWSPSTARTUP pfnWSPStartup = NULL;
	pfnWSPStartup = (LPWSPSTARTUP)::GetProcAddress(hModule, "WSPStartup");
	if (pfnWSPStartup == NULL)
	{
		ODS1(L" WSPStartup: GetProcAddress() failed %d \n", ::GetLastError());
		return WSAEPROVIDERFAILEDINIT;
	}
	// �����²��ṩ�ߵ�WSPStartup ����
	LPWSAPROTOCOL_INFOW pInfo = lpProtocolInfo;
	if (NextProtocolInfo.ProtocolChain.ChainLen == BASE_PROTOCOL)
		pInfo = &NextProtocolInfo;
	int nRet = pfnWSPStartup(wVersionRequested, lpWSPData, pInfo, UpcallTable, lpProcTable);
	if (nRet != ERROR_SUCCESS)
	{
		ODS1(L" WSPStartup: underlying provider's WSPStartup() failed %d \n", nRet);
		return nRet;
	}
	// �����²��ṩ�ߵĺ�����
	g_NextProcTable = *lpProcTable;
	// �޸Ĵ��ݸ��ϲ�ĺ�����Hook ����Ȥ�ĺ�����������Ϊʾ������Hook ��WSPSendTo ����
	// ��������Hook ������������WSPSocket��WSPCloseSocket��WSPConnect ��
	lpProcTable->lpWSPConnect = WSPConnect;
	FreeProvider(pProtoInfo);
	return nRet;

}

