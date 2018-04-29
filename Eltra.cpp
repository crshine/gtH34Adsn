// Eltra.cpp : implementation file
//

#include "stdafx.h"
#include "Eltra.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CEltra

CEltra::CEltra(int ComNo)
{
	m_nPortNo = ComNo;
}

CEltra::~CEltra()
{
}


BOOL CEltra::Send_RMPM_Cmd(int port,BOOL IsON)
{
	char On[]= {'O','(','0','0','1',',','1',')',0};	
	char Off[]={'O','(','0','0','1',',','0',')',0};	
	char CmdBuf[32];
	
	if(IsON)
		strcpy(CmdBuf,On);
	else
		strcpy(CmdBuf,Off);
	
	unsigned long dwBytesWrite=strlen(CmdBuf);
	ClearCommError(m_Handle,&dwErrorFlags,&ComStat);
	bWriteStat=WriteFile(m_Handle,CmdBuf,dwBytesWrite,& dwBytesWrite,NULL);
	if(!bWriteStat) return FALSE;
	
}

BOOL CEltra::CloseDev()
{
	CloseHandle(m_Handle);
	return TRUE;
}

BOOL CEltra::OpenDev()
{

	HANDLE hCom;
	
	CString Portstr;
	Portstr.Format(("COM%d:"), m_nPortNo%10);
//	hCom = CreateFile( "COM4",
	hCom = CreateFile( LPCTSTR(Portstr),
		GENERIC_READ|GENERIC_WRITE, 
		FILE_SHARE_READ, 
		NULL, 
//		OPEN_EXISTING, 
//OPEN_ALWAYS
		CREATE_NEW,
		FILE_ATTRIBUTE_NORMAL, 
		0);
	if (hCom == INVALID_HANDLE_VALUE) 
    {
        return FALSE;
    }
	
    if (!SetupComm(hCom, 1024, 512))
    {
        CloseHandle(hCom);
        return FALSE;
    }
	
/*
	hCom=CreateFile(L"COM4",	GENERIC_READ|GENERIC_WRITE, //�������д
		0, //��ռ��ʽ
		NULL,		OPEN_EXISTING, //�򿪶����Ǵ���
		0, //ͬ����ʽ
		NULL);
	
	if(hCom==(HANDLE)-1)
	{
		return FALSE;
	}
	*/
	SetupComm(hCom,200,200); //���뻺����������������Ĵ�С����100
	
	COMMTIMEOUTS TimeOuts;
	//�趨����ʱ
	TimeOuts.ReadIntervalTimeout=MAXDWORD;
	TimeOuts.ReadTotalTimeoutMultiplier=0;
	TimeOuts.ReadTotalTimeoutConstant=0;
	//�ڶ�һ�����뻺���������ݺ���������������أ�
	//�������Ƿ������Ҫ����ַ���
	
	
	//�趨д��ʱ
	TimeOuts.WriteTotalTimeoutMultiplier=100;
	TimeOuts.WriteTotalTimeoutConstant=500;
	SetCommTimeouts(hCom,&TimeOuts); //���ó�ʱ
	
	DCB dcb;
	GetCommState(hCom,&dcb);
	dcb.BaudRate=9600; //������Ϊ9600
	dcb.ByteSize=8; //ÿ���ֽ���8λ
	dcb.Parity=NOPARITY; //����żУ��λ
	//	dcb.StopBits=TWOSTOPBITS; //����ֹͣλ
	dcb.StopBits = 0; // 0,1,2-1,1.5,2 bits 
	int ret = SetCommState(hCom,&dcb);
	DWORD Errcode= GetLastError();

	PurgeComm(hCom,PURGE_TXCLEAR|PURGE_RXCLEAR);

	m_Handle = hCom;

	return TRUE;
}
