
// LSPLearnDlg.h : ͷ�ļ�
//

#pragma once

// CLSPLearnDlg �Ի���
class CLSPLearnDlg : public CDialogEx
{
// ����
public:
	CLSPLearnDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LSPLEARN_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	void OnBnClickedButton1();
	CString m_text;
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	void static Proc_ReadPipe(LPVOID pApp);
	bool CheckProcessName(CString szName);
	HANDLE m_hPipe;
};

