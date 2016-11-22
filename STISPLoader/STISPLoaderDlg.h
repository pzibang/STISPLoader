
// STISPLoaderDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"


// CSTISPLoaderDlg �Ի���
class CSTISPLoaderDlg : public CDialogEx
{
// ����
public:
	CSTISPLoaderDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_STISPLOADER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnCbnSelchangeCombo1();
	CComboBox CParity;
	CComboBox CBaudRate;
	afx_msg void OnCbnSelchangeCombo2();
	afx_msg void OnCbnSelchangeCombo3();
	CComboBox CDataBits;
	CComboBox CStopBits;
	afx_msg void OnCbnSelchangeCombo4();
	afx_msg void OnBnClickedFileselect();
};
