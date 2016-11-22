/******************** (C) COPYRIGHT 2015 STMicroelectronics ********************
* File Name          : STISPLoaderDlg.cpp
* Author             : Hang
* Version            : v0.0.0
* Date               :
* Description        : Implements the Main Dlg GUI and Functions
********************************************************************************
* More comments
*******************************************************************************/
// STISPLoaderDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "STISPLoader.h"
#include "STISPLoaderDlg.h"
#include "afxdialogex.h"
#include "STUARTBLLIB.h"
#include "Files.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/* Options Bytes Areas*/
/*
DWORD ADDR_F0_OPB = 0x1FFFF800;
DWORD ADDR_F1_OPB = 0x1FFFF800;
DWORD ADDR_F2_OPB = 0x1FFFC000;
DWORD ADDR_L1_OPB = 0x1FF80000;
DWORD ADDR_L4_OPB = 0x1FFF7800;
DWORD ADDR_XX_OPB = 0x1FFFF800;
*/
typedef enum STATE { OK, KO };

char MapFile[256];
PMAPPING pmMapping;

int TimeBO = 100;

BOOL SHOW_OK = TRUE;  // Set to TRUE/FALSE to show/hide OK status  messages
BOOL SHOW_KO = TRUE;  // Set to TRUE/FALSE to show/hide KO status  messages
int family = 0;

/*******************************************************************************************/
/* Function    : FileExist                                                                 */
/* IN          : file name                                                                 */
/* OUT         : boolean                                                                   */
/* Description : verify if the given file exists                                           */
/*******************************************************************************************/
BOOL FileExist(LPCTSTR filename)
{
	// Data structure for FindFirstFile
	WIN32_FIND_DATA findData;

	// Clear find structure
	ZeroMemory(&findData, sizeof(findData));

	// Search the file
	HANDLE hFind = FindFirstFile(filename, &findData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		// File not found
		return false;
	}

	// File found

	// Release find handle
	FindClose(hFind);
	hFind = NULL;

	// The file exists
	return true;
}

/*******************************************************************************************/
/* Function    : ParityToInt                                                               */
/* IN          : parity as string (NONE, ODD, EVEN)                                        */
/* OUT         : integer                                                                   */
/* Description : Get the integer representation of the given parity                        */
/*******************************************************************************************/
int ParityToInt(char* parity)
{
	if (strcmp(parity, "NONE") == 0) return 0;
	else if (strcmp(parity, "ODD") == 0) return 1;
	else if (strcmp(parity, "EVEN") == 0) return 2;
	else return 2;
}

/*******************************************************************************************/
/* Function    : ModeToInt                                                                 */
/* IN          : Mode as string (OFF, ECHO, LISTEN)                                        */
/* OUT         : int                                                                      */
/* Description : Get the int representation of the given string Mode                       */
/*******************************************************************************************/
int ModeToInt(char* status)
{
	if (strcmp(status, "OFF") == 0) return 0;
	else if (strcmp(status, "ECHO") == 0) return 1;
	else if (strcmp(status, "LISTEN") == 0) return 2;
	else return 0;
}

/*******************************************************************************************/
/* Function    : StatusToBool                                                              */
/* IN          : Status as string (ON, OFF)                                                */
/* OUT         : Bool                                                                      */
/* Description : Get the boolean representation of the given string ON/OFF                 */
/*******************************************************************************************/
bool StatusToBool(char* status)
{
	if (strcmp(status, "OFF") == 0) return false;
	else if (strcmp(status, "ON") == 0) return true;
	else return false;
}

BYTE Res = SUCCESS;
BYTE User, RDP, Data0, Data1, WRP0, WRP1, WRP2, WRP3;

//Initializing default serial connection parameters
int    portname = 1;
long   BaudRate = 115200;
int    DataBits = 8;
int    parity = ParityToInt("EVEN");
double nbStopBit = 1;
int    timeout = 15000;
bool   control = false; //�������

int    nsec = 0; //������������
DWORD  address = 0x00000000;
DWORD  size = 0x00000000;
char  filename[256] = "C:\\UserFiles\\Temp.hex"; //�ӵ�Ƭ���ϴ��ļ������ַ
char   devname[256] = "STM32F1_High-density_512K.STmap";
bool   Verify = FALSE;
bool   optimize = FALSE;
int    becho = 0;

char Drive[3], Dir[256], Fname[256], Ext[256];
char *ptr;

/***************************************************************************************/
/*********************************initial code above************************************/
/**************************************************************************************/
// CSTISPLoaderDlg �Ի���



CSTISPLoaderDlg::CSTISPLoaderDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSTISPLoaderDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSTISPLoaderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_COMBO1, CParity);
	DDX_Control(pDX, IDC_COMBO2, CBaudRate);
	DDX_Control(pDX, IDC_COMBO3, CDataBits);
	DDX_Control(pDX, IDC_COMBO4, CStopBits);
}

BEGIN_MESSAGE_MAP(CSTISPLoaderDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_CBN_SELCHANGE(IDC_COMBO1, &CSTISPLoaderDlg::OnCbnSelchangeCombo1)
	ON_CBN_SELCHANGE(IDC_COMBO2, &CSTISPLoaderDlg::OnCbnSelchangeCombo2)
	ON_CBN_SELCHANGE(IDC_COMBO3, &CSTISPLoaderDlg::OnCbnSelchangeCombo3)
	ON_CBN_SELCHANGE(IDC_COMBO4, &CSTISPLoaderDlg::OnCbnSelchangeCombo4)
	ON_BN_CLICKED(ID_FILESELECT, &CSTISPLoaderDlg::OnBnClickedFileselect)
END_MESSAGE_MAP()


// CSTISPLoaderDlg ��Ϣ�������

BOOL CSTISPLoaderDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
	//2016-11-21 add
	CParity.SetCurSel(0);
	CBaudRate.SetCurSel(0);
	CDataBits.SetCurSel(0);
	CStopBits.SetCurSel(0);
	//2016-11-21 add end

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CSTISPLoaderDlg::OnPaint()
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
HCURSOR CSTISPLoaderDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CSTISPLoaderDlg::OnCbnSelchangeCombo1()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString ParityVal;
	CParity.GetLBText(CParity.GetCurSel(), ParityVal);
	//CParity.SetCurSel(CParity.GetCurSel());
	parity = ParityToInt(ParityVal.GetBuffer(ParityVal.GetLength()));
	ParityVal.ReleaseBuffer();
}


void CSTISPLoaderDlg::OnCbnSelchangeCombo2()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString BaudRateVal;
	CBaudRate.GetLBText(CBaudRate.GetCurSel(), BaudRateVal);
	BaudRate = _ttoi(BaudRateVal.GetBuffer(BaudRateVal.GetLength()));
	BaudRateVal.ReleaseBuffer();
}


void CSTISPLoaderDlg::OnCbnSelchangeCombo3()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString DataBitsVal;
	CDataBits.GetLBText(CDataBits.GetCurSel(), DataBitsVal);
	DataBits = _ttoi(DataBitsVal.GetBuffer(DataBitsVal.GetLength()));
	DataBitsVal.ReleaseBuffer();
}


void CSTISPLoaderDlg::OnCbnSelchangeCombo4()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString StopBitsVal;
	CStopBits.GetLBText(CStopBits.GetCurSel(), StopBitsVal);
	nbStopBit = _ttof(StopBitsVal.GetBuffer(StopBitsVal.GetLength()));
	StopBitsVal.ReleaseBuffer();
}


void CSTISPLoaderDlg::OnBnClickedFileselect()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	TCHAR szFilter[] = _T("HEX�ļ�(*.hex)|*.hex|�����ļ�(*.*)|*.*||");

	// ������ļ��Ի���   
	CFileDialog fileDlg(TRUE, _T("hex"), NULL, 0, szFilter, this);

	CString strFilePath;

	// ��ʾ���ļ��Ի���   
	if (IDOK == fileDlg.DoModal())
	{
		// ���������ļ��Ի����ϵġ��򿪡���ť����ѡ����ļ�·����ʾ���༭����   
		strFilePath = fileDlg.GetPathName();
		strcpy(filename, strFilePath.GetBuffer(strFilePath.GetLength() + 1));
		strFilePath.ReleaseBuffer();
		//MessageBox(filename);
	}
}
