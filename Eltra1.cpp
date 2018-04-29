// Eltra.cpp : implementation file
//

#include "stdafx.h"
#include "Eltra.h"
//#include "ticketinfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define STATUS 0x3F
#define INSERT_TKT 0x3C
#define SETADDR 0x3E
#define ENABLE_TKT_INSERT 0x41
#define DISABLE_TKT_INSERT 0x42
#define READ_BC_FIELD 0x43
#define INIT 0x44
#define EJECT_TKT 0x46
#define PRT_BC_CUT_EJ 0x4C
#define PRT_TXT_CUT_EJ 0x50
#define READ_CONFIG 0x58
#define CAP_TKT 0x76

#define PRE_LENGHT 64

char SetAddr[]={0x12,0x02,0x42,0x3E,
0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x0A,
0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x0A,
0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x0A,
0x03,0x3032,0x04,0};

char BarTicket[]={0X12,0X02,0x42,0x4C,
0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x0A,
 'H', 'o', 'u', 'r', 'l', 'y', ' ', 'T', 'i', 'c', 'k', 'e', 't',0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x0A,
0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x0A,
0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x0A,
0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x0A,
0x03,0x3333,0x04,0};

char NormalCmd[]={0x12,0x02,0x42,0x44,0x03,0x3135,0x04,0};	


/////////////////////////////////////////////////////////////////////////////
// CEltra

CEltra::CEltra(int ComNo)
{
	m_nPortNo = ComNo;

//	lpTktInfo = new HTICKETINFO;
//	lpTktInfo->lpParkingInfo = NULL;
//	lpTktInfo->lpCreditInfo = NULL;

	sprintf(BCTicket.Head1,"Smaron Carpark");
	sprintf(BCTicket.Barcode,"01000123");
	sprintf(BCTicket.Line1,"TicketNo: %s",BCTicket.Barcode);
	sprintf(BCTicket.Line2,"EnterTime:");
	sprintf(BCTicket.Line3,"  2010-06-30 15:25");

	memcpy(SetAddr+4,BCTicket.Head1,strlen(BCTicket.Head1));
}

CEltra::~CEltra()
{
}

BOOL CEltra::sendtocomm(LPSTR sendbuf, int length)
{
	DWORD flag, ActualLen = 0;
	ClearCommError(m_Handle, &flag, NULL);
    PurgeComm(m_Handle,  PURGE_TXCLEAR|PURGE_RXCLEAR);
    BOOL WriteOK = WriteFile(m_Handle, sendbuf, length, &ActualLen, NULL);
    if ((WriteOK == TRUE) && (DWORD(length) == ActualLen)) 
		return TRUE;
	else
		return FALSE;
}

void CEltra::LRC(char *buf,int len)
{
	int LRC1=0,LRC2=0,tmp1=0,tmp2=0;
	for(int i=0;i<len-2;i++)
	{
		tmp1=buf[i]/16;
		tmp2=buf[i]-tmp1*16;
		LRC1^=tmp1;
		LRC2^=tmp2;
	}
	LRC1+=0x30;
	LRC2+=0x30;
	buf[len-2]=LRC1*256+LRC2;
//	buf[len-3]=LRC1;
//	buf[len-2]=LRC2;
}

BOOL CEltra::IsAckOK()
{
	Sleep(20);
	char str[100];
	memset(str,'\0',100);
	DWORD wCount=1;//读取的字节数
	BOOL bReadStat;
	for(int i=0;i<3;i++)
	{
		Sleep(500);
		bReadStat=ReadFile(m_Handle,str,wCount,&wCount,NULL);
		if(!bReadStat) return FALSE;

		PurgeComm(m_Handle, PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
		if(wCount>0)
		{
			if (str[0]==6) return TRUE;
			else if (str[0]==0x15) return FALSE;
			else return FALSE;
		}
	}
	return FALSE;
}
BOOL CEltra::AckData()
{
//STX DEST ECHO CODE DATA ETX LRC EOT
//02  42                  03      04 

//ECHO: Replay command char
//CODE: Chr1:	30-command accepted, 52-refused,54 not available
//		Chr2,Chr3: 3X,3Y	
	char str[100];
	memset(str,'\0',100);
	DWORD wCount=100;//读取的字节数
	BOOL bReadStat;
	CString tmpstr;
	Sleep(100);
	bReadStat=ReadFile(m_Handle,str,wCount,&wCount,NULL);
	
	if(!bReadStat) return FALSE;

	PurgeComm(m_Handle, PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
	if(wCount==1)
	{
//		m_disp.Format("%x",str[0]);
//		UpdateData(FALSE);
	}
	else if(wCount>1)
	{
		for(int i=0;i<wCount;i++) BarcodeNum[i]=str[i];
		BarcodeNum[i]=0;
		HaveBarcode=TRUE;
//		m_disp.Format("%s",str);
//		UpdateData(FALSE);
		return TRUE;
	}
	return FALSE;
}

BOOL CEltra::CheckCmdResult(int CmdId)
{
	unsigned char CmdBuf[]={0x12,0x05};
	char rec[PRE_LENGHT];
	memset(rec,'\0',PRE_LENGHT);
	DWORD wCount=PRE_LENGHT;//读取的字节数
	BOOL bReadStat;
	CString tmpstr;
	
	dwBytesWrite=sizeof(CmdBuf);
	ClearCommError(m_Handle,&dwErrorFlags,&ComStat);
	bWriteStat=WriteFile(m_Handle,CmdBuf,dwBytesWrite,& dwBytesWrite,NULL);
	
	Sleep(100);
	bReadStat=ReadFile(m_Handle,rec,wCount,&wCount,NULL);
	
	if(!bReadStat) return FALSE;
	
	PurgeComm(m_Handle, PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);

	switch(CmdId)
	{
	case READ_BC_FIELD:
		//not recog 02 42 43 30 34 30 03 33 34 04
		//no ticket 02 42 43 30 31 30 03 33 31 04
		if(wCount>8)
		{
			if(rec[0]==02 && rec[1]==0x42 && rec[2]==0x43 && rec[3]==0x30)
			{
				if(rec[4]==0x31 || rec[4]==0x34) return TRUE;
				else if(rec[4]==0x30 && rec[5]==0x30 && rec[wCount-1]==04 && wCount>10)
				{
					for(int i=0;i<wCount-10;i++) BarcodeNum[i]=rec[i+6];
					BarcodeNum[i]=0;
					HaveBarcode=TRUE;
					return TRUE;
				}
			}
			//		m_disp.Format("%s",str);
			//		UpdateData(FALSE);
			return TRUE;
		}
		break;
	case PRT_BC_CUT_EJ:
		if(rec[2]=='L' && rec[3]==0x30 && rec[4]==0x30 && rec[5]==0x38 )	return TRUE; //ticket in mouth position
		break;
	case STATUS:
		if(rec[2]=='?' && wCount>=10)
		{
			for(int i=0;i<3;i++) RecData[i]=rec[i+3];
			return TRUE;
		}
		break;
	default:
		if(rec[2]==CmdId )	return TRUE; 
		break;
	}
	
	if(wCount==1)
	{
//		m_disp.Format("%x",str[0]);
//		UpdateData(FALSE);
	}
	return FALSE;
}

BOOL CEltra::SendInquiryCmd()
{
	unsigned char CmdBuf[]={0x12,0x05};
	
	dwBytesWrite=sizeof(CmdBuf);
	ClearCommError(m_Handle,&dwErrorFlags,&ComStat);
	bWriteStat=WriteFile(m_Handle,CmdBuf,dwBytesWrite,& dwBytesWrite,NULL);
//	if(!bWriteStat) {	AfxMessageBox("写串口失败!");	}
	
	AckData();
	return TRUE;
}

// Combine datas of Discount ticket
//Input: ticketNO	;free hours		;printing time	;
BOOL CEltra::CombineDisTktData(char *ticketNo, char *prntime,int hours)
{
	char tmp[20];
	sprintf(tmp,"018%s%02d001",ticketNo,hours);
	// chech data lenght and format
	memcpy(BarTicket+4,tmp,strlen(tmp));	//barcode numbers

	sprintf(tmp,"Discount Ticket");
	memcpy(BarTicket+4+17,tmp,strlen(tmp));	//text line1

	sprintf(tmp,"TicketNo: F%s",ticketNo);
	memcpy(BarTicket+4+17+21,tmp,strlen(tmp));	//text line2

	sprintf(tmp,"Discount: %02d hrs",hours);
	memcpy(BarTicket+4+17+21+21,tmp,strlen(tmp));	//text line3

	sprintf(tmp,"PrintDate:%s",prntime);
	memcpy(BarTicket+4+17+21+21+21,tmp,strlen(tmp));//text line4
	return TRUE;
}

// Combine datas of hourly ticket
BOOL CEltra::CombineTktData(char *ticketNo, char *entertime)
{
	char tmp[20];
	sprintf(tmp,"010%s00001",ticketNo);
	// chech data lenght and format
	memcpy(BarTicket+4,tmp,strlen(tmp));	//barcode numbers

	sprintf(tmp,"Hourly Ticket    ");
	memcpy(BarTicket+4+17,tmp,strlen(tmp));	//text line1

	sprintf(tmp,"TicketNo: B%s",ticketNo);
	memcpy(BarTicket+4+17+21,tmp,strlen(tmp));	//text line2
	sprintf(tmp,"EnterTime:         ");
	memcpy(BarTicket+4+17+21+21,tmp,strlen(tmp));	//text line3
	
	sprintf(tmp,"  %s    ",entertime);
	memcpy(BarTicket+4+17+21+21+21,tmp,strlen(tmp));//text line4
	return TRUE;
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

BOOL CEltra::SendPrintCmd(int CmdId)
{
//	unsigned char CmdBuf[]={0x12,0x02,0x42,'D',0x03,0,0x04,0,0,0,0,0,0,0,0};	
	char PrintBar2[]={0X12,0X02,0x42,0x4C,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x0A,
		0x4C,0x69,0x6E,0x65,0x20,0x54,0x65,0x78,0x74,0x20,0x31,0x0A,0x4C,0x69,0x6E,0x65,0x20,0x54,0x45,0x58,0x54,0x20,0x32,0x0A,
		0x4C,0x49,0x4E,0x45,0x20,0x54,0x45,0x58,0x54,0x20,0x33,0x0A,0x4C,0x49,0x4E,0x45,0x20,0x74,0x65,0x78,0x74,0x20,0x34,0x0A,0x03,0x3333,0x04};
	char InsertTkt[]={0x12,0x02,0x42,0x3C,0x03,0x363D,0x04,0};
	char ReadBarcode[]={0x12,0x02,0x42,0x43,0x03,0x3132,0x04,0};
	char Eject[]={0x12,0x02,0x42,0x46,0x03,0x3137,0x04,0};
	char InitDev[]={0x12,0x02,0x42,0x44,0x03,0x3135,0x04,0};	//init 12 02 42 44 03 3135 04
	char Status[]={0x12,0x02,0x42,0x3f,0x03,0x3135,0x04,0};	
	char CmdBuf[256];	
	if(CmdId==PRT_BC_CUT_EJ ||CmdId==PRT_TXT_CUT_EJ)
		strcpy(CmdBuf,BarTicket);	
	else if(CmdId==SETADDR)
		strcpy(CmdBuf,SetAddr);	
	else 
	{
		strcpy(CmdBuf,NormalCmd);
		CmdBuf[3]=CmdId;
	}
	LRC(CmdBuf,strlen(CmdBuf));

	dwBytesWrite=strlen(CmdBuf);
	ClearCommError(m_Handle,&dwErrorFlags,&ComStat);
	bWriteStat=WriteFile(m_Handle,CmdBuf,dwBytesWrite,& dwBytesWrite,NULL);
	if(!bWriteStat) return FALSE;

	if(CmdId==PRT_BC_CUT_EJ) Sleep(1000); //IsackOK return false
	
	if(!IsAckOK())
		if(CmdId!=PRT_BC_CUT_EJ)
			return FALSE;

	if(CmdId==READ_BC_FIELD) {HaveBarcode=FALSE; }

	Sleep(200);

	BOOL CmdOK = CheckCmdResult(CmdId);
	if(!CmdOK && CmdId==READ_BC_FIELD)
//		while(!CheckCmdResult(CmdId) );

	for (int i=0;i<9;i++)
	{
		Sleep(1000);
		CmdOK = CheckCmdResult(CmdId);
		if(CmdOK) break;
	}

	if(!CmdOK )
//	if(!CmdOK && CmdId==PRT_BC_CUT_EJ)
		for (int i=0;i<2;i++)
		{
			Sleep(1000);
			CmdOK = CheckCmdResult(CmdId);
			if(CmdOK) break;
		}

	if(!CmdOK) return FALSE;

	return TRUE;
}


BOOL CEltra::Test()
{
	int len;
	char PrintBar0[]={0x12,0x02,0x42,0x4C,0};
	char PrintBar[]={0x12,0x02,0x42,0x4C,0x31,0x31,0x31,0x31,0x0A,'B','A','A','A',0x0A,0x03,0x313d,0x04,0};
	char PrintBar2[]={0X12,0X02,0x42,0x4C,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x0A,
		0x4C,0x69,0x69,0x69,0x69,0x69,0x69,0x69,0x69,0x69,'A',0x65,0x20,0x54,0x65,0x78,0x74,0x20,0x31,0x0A,
		0x4C,0x69,0x6E,0x65,0x20,0x54,0x45,0x58,0x54,0x20,0x32,0x0A,
		0x4C,0x49,0x4E,0x45,0x20,0x54,0x45,0x58,0x54,0x20,0x33,0x0A,
		0x4C,0x49,0x4E,0x45,0x20,0x74,0x65,0x78,0x74,0x20,0x34,0x0A,0x03,0x3333,0x04,0};

	char CmdBuf[256];	
	{
		strcpy(CmdBuf,PrintBar2);
//		memcpy(CmdBuf+21,BCTicket.Line1,strlen(BCTicket.Line1));
		LRC(CmdBuf,strlen(CmdBuf));
	}
//return FALSE;
	
	//	dwBytesWrite=sizeof(CmdBuf);
	dwBytesWrite=strlen(CmdBuf);
	ClearCommError(m_Handle,&dwErrorFlags,&ComStat);
	bWriteStat=WriteFile(m_Handle,CmdBuf,dwBytesWrite,& dwBytesWrite,NULL);
	if(!bWriteStat) return FALSE;
	
	if(!IsAckOK())
	{
		return FALSE;
	}
	Sleep(200);
	//	SendInquiryCmd();
	
	BOOL CmdOK=CheckCmdResult(PRT_BC_CUT_EJ);
	
	if(!CmdOK )
		CmdOK=CheckCmdResult(PRT_BC_CUT_EJ);
//		while(!CheckCmdResult(PRT_BC_CUT_EJ)) ;
		
		return TRUE;
}


BOOL CEltra::El_ReadBarcode(char *bar)
{
	if(SendPrintCmd(READ_BC_FIELD))
		if(HaveBarcode)
		{
			strcpy(bar,BarcodeNum);
			return TRUE;
		}
		return FALSE;
}

BOOL CEltra::OpenDev()
{

	HANDLE hCom;
	
	CString Portstr;
	Portstr.Format(_T("COM%d:"), m_nPortNo%10);
	hCom = CreateFile( LPCTSTR(Portstr),
		GENERIC_READ|GENERIC_WRITE, 
		FILE_SHARE_READ, 
		NULL, 
		OPEN_EXISTING, 
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
	hCom=CreateFile(L"COM1",	GENERIC_READ|GENERIC_WRITE, //允许读和写
		0, //独占方式
		NULL,		OPEN_EXISTING, //打开而不是创建
		0, //同步方式
		NULL);
	
	if(hCom==(HANDLE)-1)
	{
		return FALSE;
	}
	
	SetupComm(hCom,200,200); //输入缓冲区和输出缓冲区的大小都是100
*/	
	COMMTIMEOUTS TimeOuts;
	//设定读超时
	TimeOuts.ReadIntervalTimeout=MAXDWORD;
	TimeOuts.ReadTotalTimeoutMultiplier=0;
	TimeOuts.ReadTotalTimeoutConstant=0;
	//在读一次输入缓冲区的内容后读操作就立即返回，
	//而不管是否读入了要求的字符。
	
	
	//设定写超时
	TimeOuts.WriteTotalTimeoutMultiplier=100;
	TimeOuts.WriteTotalTimeoutConstant=500;
	SetCommTimeouts(hCom,&TimeOuts); //设置超时
	
	DCB dcb;
	GetCommState(hCom,&dcb);
	dcb.BaudRate=9600; //波特率为9600
	//	dcb.BaudRate=115200; //波特率为9600
	dcb.ByteSize=8; //每个字节有8位
	dcb.Parity=NOPARITY; //无奇偶校验位
	//	dcb.StopBits=TWOSTOPBITS; //两个停止位
	dcb.StopBits=0; //1个停止位
	int ret = SetCommState(hCom,&dcb);
	
	PurgeComm(hCom,PURGE_TXCLEAR|PURGE_RXCLEAR);

	m_Handle = hCom;

//KDE
/*
	m_Handle = INVALID_HANDLE_VALUE;
	if ((m_nPortNo > 4) && (m_nPortNo < 1)) return FALSE;
	HANDLE tmpID;
	CString Portstr;
	Portstr.Format(_T("COM%d:"), m_nPortNo%10);
	tmpID = CreateFile( LPCTSTR(Portstr),
		GENERIC_READ|GENERIC_WRITE, 
		FILE_SHARE_READ, 
		NULL, 
		OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, 
		0);
	if (tmpID == INVALID_HANDLE_VALUE) 
    {
        return FALSE;
    }
	
    if (!SetupComm(tmpID, 1024, 512))
    {
        CloseHandle(tmpID);
        return FALSE;
    }
	
	COMMTIMEOUTS TimeOuts;
	TimeOuts.ReadIntervalTimeout = MAXDWORD;
	TimeOuts.ReadTotalTimeoutMultiplier = 0;
	TimeOuts.ReadTotalTimeoutConstant = 0;
	TimeOuts.WriteTotalTimeoutMultiplier = 10;
	TimeOuts.WriteTotalTimeoutConstant = 100;
	if (!SetCommTimeouts(tmpID, &TimeOuts))
	{
        CloseHandle(tmpID);
        return FALSE;
    }
	
	DCB thedcb;
	//SET DCB 9600:n, 8, 1 WITHOUT ANY CONTROL
	GetCommState(tmpID, &thedcb);
	thedcb.BaudRate = 9600;
	thedcb.fBinary = TRUE;
	thedcb.ByteSize = 8;
	thedcb.fParity = FALSE;
	thedcb.Parity = NOPARITY;
	thedcb.fErrorChar = FALSE;
	thedcb.fNull = FALSE;
	thedcb.fAbortOnError = FALSE;
	thedcb.StopBits = ONESTOPBIT;
	thedcb.fDsrSensitivity = FALSE;
	thedcb.fOutxDsrFlow = FALSE;
	thedcb.fOutxCtsFlow = FALSE;
	thedcb.fDtrControl = DTR_CONTROL_DISABLE;
	thedcb.fRtsControl = RTS_CONTROL_DISABLE;
	thedcb.fTXContinueOnXoff = TRUE;
	thedcb.fInX = FALSE;
	thedcb.fOutX = FALSE;
	thedcb.wReserved = 0;
    if (!SetCommState(tmpID, &thedcb))
    {
        CloseHandle(tmpID);
        return FALSE;
    }
	
    if (!PurgeComm(tmpID, PURGE_TXCLEAR|PURGE_RXCLEAR))
    {
        CloseHandle(tmpID);
        return FALSE;
    }
	
	m_Handle = tmpID;
	*/	
	return TRUE;
}

BOOL CEltra::IsTktActive()
{
	if(!SendPrintCmd(STATUS)) return FALSE;
	if (RecData[1]==0x32) return TRUE;
	else return FALSE;
}

int CEltra::El_Status()
{
	// 0x30-NULL,32-mouth,36-inner,
	if(!SendPrintCmd(STATUS)) return -1;
	//	return RecData[1]-0x30;
	if (RecData[1]==0x30) return 0;
	else if (RecData[1]==0x32) return 2;
	else if (RecData[1]==0x36) return 6;
	else return 0;
}


BOOL CEltra::El_Init()
{
	if(!SendPrintCmd(INIT)) return FALSE;

	return TRUE;
}

BOOL CEltra::El_Insert()
{
	if(!SendPrintCmd(INSERT_TKT)) return FALSE;
	return TRUE;
}

BOOL CEltra::El_Eject()
{
	if(!SendPrintCmd(EJECT_TKT)) return FALSE;
	return TRUE;
}

BOOL CEltra::El_Capture()
{
	if(!SendPrintCmd(CAP_TKT)) return FALSE;
	return TRUE;
}

BOOL CEltra::El_Print(char *Tkt_NO,char *Entertime)
{
	if(CombineTktData(Tkt_NO,Entertime))
		if(!SendPrintCmd(PRT_BC_CUT_EJ)) 
			return FALSE;
	return TRUE;
}

BOOL CEltra::El_PrintDiscountTkt(char *Tkt_NO,char *Prntime,int freehours)
{
	if(CombineDisTktData(Tkt_NO,Prntime,freehours))
		if(!SendPrintCmd(PRT_BC_CUT_EJ)) 
			return FALSE;
		return TRUE;
}

BOOL CEltra::El_SetAddr()
{
	memcpy(SetAddr+4,BCTicket.Head1,strlen(BCTicket.Head1));
	LRC(SetAddr,strlen(SetAddr));

	if(!SendPrintCmd(SETADDR)) return FALSE;
	return TRUE;
}
