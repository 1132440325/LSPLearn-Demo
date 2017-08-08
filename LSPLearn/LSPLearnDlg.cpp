
// LSPLearnDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "LSPLearn.h"
#include "LSPLearnDlg.h"
#include "afxdialogex.h"
#include <MyDebugTools.h>
#include <WS2spi.h>
#include <SpOrder.h>
#include "LspHelper.h"
#include <thread>
#include <MySocks5ClientHelper.h>

#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib,"Rpcrt4.lib")
int InstallProvider(WCHAR *wszDllPath);

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CLSPLearnDlg �Ի���


CLSPLearnDlg::CLSPLearnDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_LSPLEARN_DIALOG, pParent)
	, m_text(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLSPLearnDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_text);
}

BEGIN_MESSAGE_MAP(CLSPLearnDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CLSPLearnDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CLSPLearnDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CLSPLearnDlg::OnBnClickedButton3)
END_MESSAGE_MAP()


// CLSPLearnDlg ��Ϣ�������

BOOL CLSPLearnDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��


	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Proc_ReadPipe, this, 0, 0);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CLSPLearnDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CLSPLearnDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CLSPLearnDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CLSPLearnDlg::OnBnClickedButton1()
{
	DWORD dwSize = 0;
	WSAPROTOCOL_INFO ProtoInfo = { 0 };
	WSAEnumProtocols(NULL, &ProtoInfo, &dwSize);
	if (dwSize <= 0) {
		return;
	}
	LPWSAPROTOCOL_INFO pProtoInfos = new WSAPROTOCOL_INFO[dwSize / sizeof(WSAPROTOCOL_INFO)];
	DWORD iCount = WSAEnumProtocols(NULL, pProtoInfos, &dwSize);
	if (pProtoInfos != NULL) {
		for (int i = 0; i < iCount; i++) {
			tssc(pProtoInfos->szProtocol);
			m_text += CString(pProtoInfos->szProtocol) + L"\r\n";
			pProtoInfos++;
		}
	}
	pProtoInfos -= iCount;
	delete pProtoInfos;
	UpdateData(FALSE);
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

void CLSPLearnDlg::OnBnClickedButton2()
{
	WCHAR buffer[MAX_PATH + 1] = {0};
	GetCurrentDirectory(MAX_PATH, buffer);
	CString path(buffer);
	path += L"\\LSPDLL.dll";
	InstallProvider(path.GetBuffer());
}

GUID ProviderGuid = { 0xd3c21122, 0x85e1, 0x48f3,{ 0x9a,0xb6,0x23,0xd9,0x0c,0x73,0x07,0xef } };

int InstallProvider(WCHAR *wszDllPath)
{
	int i = 0;
	WCHAR wszLSPName[] = L"MyLsp"; // ���ǵ�LSP ������
	int nError = NO_ERROR;
	LPWSAPROTOCOL_INFOW pProtoInfo;
	int nProtocols;
	WSAPROTOCOL_INFOW UDPLayeredInfo, UDPChainInfo; // ����Ҫ��װ��UDP �ֲ�Э���Э����
	DWORD dwUdpOrigCatalogId, dwLayeredCatalogId;
	// ��Winsock Ŀ¼���ҵ�ԭ����UDP �����ṩ�ߣ����ǵ�LSP Ҫ��װ����֮��
	// ö�����з�������ṩ��
	pProtoInfo = GetProvider(&nProtocols); // �˺�����ʵ����ο�����ǰ�������
	for (int i = 0; i < nProtocols; i++)
	{
		if (pProtoInfo[i].iAddressFamily == AF_INET && pProtoInfo[i].iProtocol == IPPROTO_TCP)
		{
			memcpy(&UDPChainInfo, &pProtoInfo[i], sizeof(UDPLayeredInfo));
			// ȥ��XP1_IFS_HANDLES ��־
			UDPChainInfo.dwServiceFlags1 = UDPChainInfo.dwServiceFlags1 & ~XP1_IFS_HANDLES;
			// ����ԭ�������ID
			dwUdpOrigCatalogId = pProtoInfo[i].dwCatalogEntryId;
			break;
		}
	}
	// ���Ȱ�װ�ֲ�Э�飬��ȡһ��Winsock �ⰲ�ŵ�Ŀ¼ID �ţ���dwLayeredCatalogId
	// ֱ��ʹ���²�Э���WSAPROTOCOL_INFOW�ṹ����
	memcpy(&UDPLayeredInfo, &UDPChainInfo, sizeof(UDPLayeredInfo));
	// �޸�Э�����ƣ����ͣ�����PFL_HIDDEN ��־
	wcscpy(UDPLayeredInfo.szProtocol, wszLSPName);
	UDPLayeredInfo.ProtocolChain.ChainLen = LAYERED_PROTOCOL; // LAYERED_PROTOCOL��0
	UDPLayeredInfo.dwProviderFlags |= PFL_HIDDEN;
	// ��װ
	if (::WSCInstallProvider(&ProviderGuid,
		wszDllPath, &UDPLayeredInfo, 1, &nError) == SOCKET_ERROR)
		return nError;
	// ����ö��Э�飬��ȡ�ֲ�Э���Ŀ¼ID ��
	FreeProvider(pProtoInfo);
	pProtoInfo = GetProvider(&nProtocols);
	for (i = 0; i < nProtocols; i++)
	{
		if (memcmp(&pProtoInfo[i].ProviderId, &ProviderGuid, sizeof(ProviderGuid)) == 0)
		{
			dwLayeredCatalogId = pProtoInfo[i].dwCatalogEntryId;
			break;
		}
	}
	// ��װЭ����
	// �޸�Э�����ƣ�����
	WCHAR wszChainName[WSAPROTOCOL_LEN + 1];
	swprintf(wszChainName, L"%ws over %ws", wszLSPName, UDPChainInfo.szProtocol);
	wcscpy(UDPChainInfo.szProtocol, wszChainName);
	if (UDPChainInfo.ProtocolChain.ChainLen == 1)
	{
		UDPChainInfo.ProtocolChain.ChainEntries[1] = dwUdpOrigCatalogId;
	}
	else
	{
		for (i = UDPChainInfo.ProtocolChain.ChainLen; i > 0; i--)
		{
			UDPChainInfo.ProtocolChain.ChainEntries[i] = UDPChainInfo.ProtocolChain.ChainEntries[i - 1];
		}
	}
	UDPChainInfo.ProtocolChain.ChainLen++;
	// �����ǵķֲ�Э�����ڴ�Э�����Ķ���
		UDPChainInfo.ProtocolChain.ChainEntries[0] = dwLayeredCatalogId;
	// ��ȡһ��Guid����װ֮
	GUID ProviderChainGuid;
	if (::UuidCreate(&ProviderChainGuid) == RPC_S_OK)
	{
		if (::WSCInstallProvider(&ProviderChainGuid,
			wszDllPath, &UDPChainInfo, 1, &nError) == SOCKET_ERROR)
			return nError;
	}
	else
		return GetLastError();
	// ��������Winsock Ŀ¼�������ǵ�Э������ǰ
	// ����ö�ٰ�װ��Э��
	FreeProvider(pProtoInfo);
	pProtoInfo = GetProvider(&nProtocols);
	DWORD dwIds[20];
	int nIndex = 0;
	// ������ǵ�Э����
	for (i = 0; i < nProtocols; i++)
	{
		if ((pProtoInfo[i].ProtocolChain.ChainLen > 1) &&
			(pProtoInfo[i].ProtocolChain.ChainEntries[0] == dwLayeredCatalogId))
			dwIds[nIndex++] = pProtoInfo[i].dwCatalogEntryId;
	}
	// �������Э��
	for (i = 0; i < nProtocols; i++)
	{
		if ((pProtoInfo[i].ProtocolChain.ChainLen <= 1) ||
			(pProtoInfo[i].ProtocolChain.ChainEntries[0] != dwLayeredCatalogId))
			dwIds[nIndex++] = pProtoInfo[i].dwCatalogEntryId;
	}
	// ��������Winsock Ŀ¼
	nError = ::WSCWriteProviderOrder(dwIds, nIndex);
	FreeProvider(pProtoInfo);
	return nError;
}

void RemoveProvider()
{
	int i = 0;
	LPWSAPROTOCOL_INFOW pProtoInfo;
	int nProtocols;
	DWORD dwLayeredCatalogId;
	// ����Guid ȡ�÷ֲ�Э���Ŀ¼ID ��
	pProtoInfo = GetProvider(&nProtocols);
	int nError;
	for (int i = 0; i < nProtocols; i++)
	{
		if (memcmp(&ProviderGuid, &pProtoInfo[i].ProviderId, sizeof(ProviderGuid)) == 0)
		{
			dwLayeredCatalogId = pProtoInfo[i].dwCatalogEntryId;
			break;
		}
	}
	if (i < nProtocols)
	{ // �Ƴ�Э����
		for (i = 0; i < nProtocols; i++)
		{
			if ((pProtoInfo[i].ProtocolChain.ChainLen > 1) &&
				(pProtoInfo[i].ProtocolChain.ChainEntries[0] == dwLayeredCatalogId))
			{
				::WSCDeinstallProvider(&pProtoInfo[i].ProviderId, &nError);
			}
		}
		// �Ƴ��ֲ�Э��
		::WSCDeinstallProvider(&ProviderGuid, &nError);
	}
}

void CLSPLearnDlg::OnBnClickedButton3()
{
	RemoveProvider();
}

#define MSGTYPE_PROCESS_NAME 0x1
#define MSGTYPE_PROXY_INFO 0x2
struct PipeMessage
{
	DWORD Type;
	CHAR Text[MAX_PATH + 1];
};

void CLSPLearnDlg::Proc_ReadPipe(LPVOID pApp)
{
	CLSPLearnDlg* App = (CLSPLearnDlg*)pApp;
	HANDLE m_hPipe = CreateNamedPipe(L"\\\\.\\pipe\\XueLspPipe", PIPE_ACCESS_DUPLEX, 0, 1, 1024, 1024, 0, NULL);
	ConnectNamedPipe(m_hPipe, NULL);
	WCHAR BUFFER[1024];
	DWORD dwRead = 0;
	while (TRUE)
	{
		ZeroMemory(BUFFER, 1024 * 2);
		if (!ReadFile(m_hPipe, BUFFER, 1024, &dwRead, NULL))
		{
			DisconnectNamedPipe(m_hPipe);
			CloseHandle(m_hPipe);
			HANDLE m_hPipe = CreateNamedPipe(L"\\\\.\\pipe\\XueLspPipe",
				PIPE_ACCESS_DUPLEX, 0, 255, 1024, 1024, 0, NULL);
			ConnectNamedPipe(m_hPipe, NULL);
			App->m_text += CString(L"[+]�ؽ��ܵ�") + L"\r\n";
		}
		if (dwRead > 0) {
			PipeMessage* pPmsg = (PipeMessage*)BUFFER;
			if (pPmsg->Type == MSGTYPE_PROCESS_NAME) {
				//�ȶԽ��������Ƿ���Ҫ���д���
				CStringA ret;
				if (App->CheckProcessName(CString(pPmsg->Text))) {
					ret = "1";
				} else {
					ret = "-1";
				}
				dwRead = 0;
				WriteFile(m_hPipe, ret.GetBuffer(), ret.GetLength(), &dwRead, NULL);
			}
			else if (pPmsg->Type == MSGTYPE_PROXY_INFO) {
				SOCKADDR_IN *psi = (SOCKADDR_IN*)pPmsg->Text;
				CHAR* s5t = inet_ntoa((*psi).sin_addr);
				OutputDebugStringA(s5t);
				//�׽���ȡ����˿�
				SOCKET temp_socket = socket(AF_INET, SOCK_STREAM, 0);
				SOCKADDR_IN si;
				si.sin_family = AF_INET;
				si.sin_port = 0;
				si.sin_addr.S_un.S_addr = INADDR_ANY;
				if (bind(temp_socket, (SOCKADDR*)&si, sizeof(si)) == SOCKET_ERROR)
				{
					OutputDebugStringW(L"bind socket error!");
					return;
				}
				listen(temp_socket, 5);
				int isize = sizeof(si);
				getsockname(temp_socket, (SOCKADDR*)&si, &isize);
				//ȡ���˿�
				int iPort = ntohs(si.sin_port);
				CStringA ret;
				ret.Format("%d", iPort);
				WriteFile(m_hPipe, ret.GetBuffer(), ret.GetLength(), &dwRead, NULL);
				//������ɣ������̴߳���
				std::thread t1([](SOCKET m_socket, SOCKADDR_IN si)
				{
					SOCKET m_accpet = accept(m_socket, NULL,NULL);
					if (m_accpet == INVALID_SOCKET) {
						OutputDebugStringW(L"Accept Socket Error");
						return;
					}
					OutputDebugStringW(L"Get a client connect��");
					//�ɹ����ܿͻ����׽���
					Socks5Client sc;
					sc.Init(L"192.168.199.242",1080,L"admin", L"admin");
					CHAR* s5t = inet_ntoa(si.sin_addr);
					OutputDebugStringA(s5t);
					CString StrDbg;
					StrDbg.Format(L"Socks5 ����������%s:%d", s5t, ntohs(si.sin_port));
					OutputDebugStringW(StrDbg);
					if (!sc.Connect()) {
						OutputDebugStringW(L"Sockets connect fail!");
					}
					SOCKET s5_socket = sc.Socket_Connect(si);
					if (s5_socket == INVALID_SOCKET) {
						OutputDebugStringW(L"Socket connect target fail!");
					}
					auto proc_swap = [](SOCKET s1, SOCKET s2) {
						CHAR buffer[1024];
						while (true)
						{
							OutputDebugStringW(L"begin swap data");
							ZeroMemory(buffer, 1024);
							if (recv(s1, buffer, 1024, 0) <= 0) { OutputDebugStringW(L"Socket Swap Error Exit!"); break; }
							OutputDebugStringA(buffer);
							if (send(s2, buffer, 1024, 0) <= 0) { OutputDebugStringW(L"Socket Swap Error Exit!"); break; }
						}
					};
					std::thread swap_0(proc_swap, m_accpet, s5_socket);
					std::thread swap_1(proc_swap, s5_socket,m_accpet);
					swap_0.detach();
					swap_1.detach();
				}, temp_socket, *psi);
				t1.detach();
			}
		}
	}
}


bool CLSPLearnDlg::CheckProcessName(CString szName)
{
	if (szName.Mid(szName.ReverseFind('\\')+1) == L"1.exe") {
		return true;
	}
	if (szName.Mid(szName.ReverseFind('\\') + 1) == L"2.exe") {
		return true;
	}
	return false;
}

