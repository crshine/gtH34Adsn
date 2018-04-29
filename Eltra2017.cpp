// Eltra.cpp : implementation file
//

#include "stdafx.h"
#include "Eltra2017.h"
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
//#define READ_BC_FIELD 0x43
#define READ_BC_FIELD 0x23//2017- new fw
#define INIT 0x44
//#define EJECT_TKT 0x46
#define EJECT_TKT 0x40
#define PRT_BC_CUT_EJ 0x4C
#define PRT_TXT_CUT_EJ 0x50
#define READ_CONFIG 0x58
#define CAP_TKT 0x76

#define PRE_LENGHT 64

/* // New Firmware Create commmand data 2017
1FF001E0251005A0PARKING TICKET

1FF003c0251005A0T1 / 252020000001176701

1FF005a0251005A0DATE 19/11/13 TIME 09:53

1FE00800251005A0PARKING CENTAR

010009800AB0000501234567812345678

1FE01280251005A0LOST TICKET   5 EURO

1FE01460251005A0THANKS FOR YOUR VISIT
*/
/* Validate Command datas
000009A002D00005056789056

1FF006001B4010E2Nr.:

1FF004001B4010E2EXIT:
1FF002001B4010E2GATE:

1FE006001F4010E256789056

1FE004001F4010E212/12/2013

1FE002001F4010E2B1
char ValidateData1[]="000009A002D00005056789056\n";
char ValidateData2[]="1FF006001B4010E2Nr.:\n";
char ValidateData3[]="1FF004001B4010E2EXIT:\n";
char ValidateData4[]="1FF002001B4010E2GATE:\n";
char ValidateData5[]="1FE006001F4010E256789056\n";
char ValidateData6[]="1FE004001F4010E212/12/2013\n";
char ValidateData7[]="1FE002001F4010E2B1\n";
*/

char ValidateData1[]="000009A002D00005056789056";
char ValidateData2[]="1FF006001B4010E2Nr.:";
char ValidateData3[]="1FF004001B4010E2EXIT:";
char ValidateData4[]="1FF002001B4010E2GATE:";
//char ValidateData5[]="1FE006001F4010E256789056";//org- ok
char ValidateData5[]="1FE006001F4010E2567890";
// char ValidateData6[]="1FE004001F4010E212/12/2013";
char ValidateData6[]="1FE004002F4010E212/12/2013";
char ValidateData7[]="1FE002001F4010E2B1";

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

extern BOOL DemoEntry;

/////////////////////////////////////////////////////////////////////////////
// CEltra2017

CEltra2017::CEltra2017(int ComNo)
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

CEltra2017::~CEltra2017()
{
}

BOOL CEltra2017::sendtocomm(LPSTR sendbuf, int length)
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

void CEltra2017::LRC(char *buf,int len)
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

BOOL CEltra2017::IsAckOK()
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
BOOL CEltra2017::AckData()
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

BOOL CEltra2017::CheckCmdResult(int CmdId)
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
			if(rec[0]==02 && rec[1]==0x42 && rec[2]==0x23 && rec[3]==0x30)
//				if(rec[0]==02 && rec[1]==0x42 && rec[2]==0x43 && rec[3]==0x30)
			{
				CString buf;
				buf.Format("%d,%d,%d,%d",rec[4],rec[5],rec[6],0);
//				MessageBox(NULL,buf,"read",0);
				if(rec[4]==0x31 || rec[4]==0x34) return TRUE;
				else if(rec[4]==0x30 && rec[5]==0x30 && rec[wCount-1]==04 && wCount>10)
				{
					buf.Format("%d,%d,%d,%d",rec[6],rec[7],rec[8],wCount);
	//				MessageBox(NULL,buf,"find",0);
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
// 		MessageBox(NULL,"no","",0);
		break;
	case PRT_BC_CUT_EJ:
		if(rec[2]=='L' && rec[3]==0x30 && rec[4]==0x30 && rec[5]==0x38 )	return TRUE; //ticket in mouth position
		break;
	case STATUS:
		if(rec[2]=='?' && wCount>=10)
		{
//			for(int i=0;i<3;i++) RecData[i]=rec[i+3];//old
			for(int i=0;i<6;i++) RecData[i]=rec[i+3];//2017
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

BOOL CEltra2017::SendInquiryCmd()
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
BOOL CEltra2017::CombineDisTktData(char *ticketNo, char *prntime,int hours)
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
#ifdef _DEBUG
	sprintf(tmp,"PrintDate:%s",prntime);
#else
	sprintf(tmp,"PrintDate:  %s",prntime);
#endif
	memcpy(BarTicket+4+17+21+21+21,tmp,strlen(tmp));//text line4
	return TRUE;
}
BOOL CEltra2017::CombineDisTktData(char *ticketNo, char *prntime,char *validtodate)
{
	char tmp[21];
	char tmpdate[8];
	tmpdate[0]=validtodate[2];
	tmpdate[1]=validtodate[3];
	tmpdate[2]=validtodate[5];
	tmpdate[3]=validtodate[6];
	tmpdate[4]=validtodate[8];
	tmpdate[5]=validtodate[9];
	tmpdate[6]=0;
	sprintf(tmp,"018%s%s1",ticketNo+2,tmpdate);
	// chech data lenght and format
	memcpy(BarTicket+4,tmp,strlen(tmp));	//barcode numbers
	
	sprintf(tmp,"Discount Ticket");
	sprintf(tmp,"Discount Ticket 2H",ticketNo[2]); //BLW- testing
	memcpy(BarTicket+4+17,tmp,strlen(tmp));	//text line1
	
	sprintf(tmp,"TicketNo: F%s",ticketNo);
	memcpy(BarTicket+4+17+21,tmp,strlen(tmp));	//text line2
	
	sprintf(tmp,"PrintDate:%s",prntime);
	memcpy(BarTicket+4+17+21+21,tmp,strlen(tmp));	//text line3
	
	sprintf(tmp,"ValidTo  :%s",validtodate);
	memcpy(BarTicket+4+17+21+21+21,tmp,strlen(tmp));//text line4
//MessageBox(NULL,BarTicket,"1",0);
	return TRUE;
}

BOOL CEltra2017::CombineDisTktData(char *ticketNo, char *prntime,char *validtodate,int freehour)
{
	char tmp[21];
	char tmpdate[8];
	tmpdate[0]=validtodate[2];
	tmpdate[1]=validtodate[3];
	tmpdate[2]=validtodate[5];
	tmpdate[3]=validtodate[6];
	tmpdate[4]=validtodate[8];
	tmpdate[5]=validtodate[9];
	tmpdate[6]=0;
	sprintf(tmp,"018%s%s1",ticketNo+2,tmpdate);
	sprintf(tmp,"018%s%s%1d",ticketNo+2,tmpdate,freehour);//BLW - use the last byte as discount hour,
	// chech data lenght and format
	memcpy(BarTicket+4,tmp,strlen(tmp));	//barcode numbers
	
	sprintf(tmp,"Discount Ticket");
	sprintf(tmp,"Discount Ticket %1dH  ",freehour); //BLW- testing
	if(freehour==0) 
		sprintf(tmp,"Discount Ticket  All"); //BLW- testing

	memcpy(BarTicket+4+17,tmp,strlen(tmp));	//text line1
	
	sprintf(tmp,"TicketNo: F%s",ticketNo);
	memcpy(BarTicket+4+17+21,tmp,strlen(tmp));	//text line2
	
	sprintf(tmp,"PrintDate:%s",prntime);
	memcpy(BarTicket+4+17+21+21,tmp,strlen(tmp));	//text line3
	
	sprintf(tmp,"ValidTo  :%s",validtodate);
	memcpy(BarTicket+4+17+21+21+21,tmp,strlen(tmp));//text line4
	//MessageBox(NULL,BarTicket,"1",0);
	return TRUE;
}

// Combine datas of hourly ticket
BOOL CEltra2017::CombineTktData(char *ticketNo, char *entertime)
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

BOOL CEltra2017::Send_RMPM_Cmd(int port,BOOL IsON)
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

BOOL CEltra2017::SendPrintCmd(int CmdId)
{
//	unsigned char CmdBuf[]={0x12,0x02,0x42,'D',0x03,0,0x04,0,0,0,0,0,0,0,0};	
	char PrintBar2[]={0X12,0X02,0x42,0x4C,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x0A,
		0x4C,0x69,0x6E,0x65,0x20,0x54,0x65,0x78,0x74,0x20,0x31,0x0A,0x4C,0x69,0x6E,0x65,0x20,0x54,0x45,0x58,0x54,0x20,0x32,0x0A,
		0x4C,0x49,0x4E,0x45,0x20,0x54,0x45,0x58,0x54,0x20,0x33,0x0A,0x4C,0x49,0x4E,0x45,0x20,0x74,0x65,0x78,0x74,0x20,0x34,0x0A,0x03,0x3333,0x04};
//	char InsertTkt[]={0x12,0x02,0x42,0x3C,0x03,0x363D,0x04,0};//org
	char InsertTkt[]={0x12,0x02,0x42,0x3C,0x3A,0x363D,0x04,0};
//	char ReadBarcode[]={0x12,0x02,0x42,0x43,0x03,0x3132,0x04,0};//org
	char ReadBarcode[]={0x12,0x02,0x42,0x23,0x03,0x30,0x30,0x04,0};
	char Eject[]={0x12,0x02,0x42,0x46,0x03,0x3137,0x04,0};
	char InitDev[]={0x12,0x02,0x42,0x44,0x03,0x3135,0x04,0};	//init 12 02 42 44 03 3135 04
//	char Status[]={0x12,0x02,0x42,0x3f,0x03,0x3135,0x04,0};	//old
	char Status[]={0x12,0x02,0x42,0x3f,0x03,0x3030,0x04,0};	//2017
	char CmdBuf[256];
	
	char CmdInsert[]={0x12,0x02,0x42,0x3c,0x30,0x31,0x30,0x30,0x0a,0x03,0x30,0x30,0x04,0};	//2017-the second barcode read ok
//	char CmdInsert[]={0x12,0x02,0x42,0x3c,0x30,0x31,0x31,0x30,0x0a,0x03,0x30,0x30,0x04,0};	//2017- new firmware

	if(CmdId==STATUS) strcpy(CmdBuf,Status);
	else
	if(CmdId==PRT_BC_CUT_EJ ||CmdId==PRT_TXT_CUT_EJ)
		strcpy(CmdBuf,BarTicket);	
	else if(CmdId==SETADDR)
		strcpy(CmdBuf,SetAddr);	
	else if(CmdId==INSERT_TKT)//2017- NEW FIRMEARE
		strcpy(CmdBuf,CmdInsert);
	else if(CmdId==READ_BC_FIELD)//2017- NEW FIRMEARE
		strcpy(CmdBuf,ReadBarcode);
	else 
	{
		strcpy(CmdBuf,NormalCmd);
		CmdBuf[3]=CmdId;
	}
	LRC(CmdBuf,strlen(CmdBuf));

	if(CmdId==READ_BC_FIELD)//2017- NEW FIRMEARE
		strcpy(CmdBuf,ReadBarcode);

	dwBytesWrite=strlen(CmdBuf);
	ClearCommError(m_Handle,&dwErrorFlags,&ComStat);
	bWriteStat=WriteFile(m_Handle,CmdBuf,dwBytesWrite,& dwBytesWrite,NULL);
	Sleep(100); //delay1
	if(!bWriteStat) return FALSE;
//MessageBox(NULL,"write","",0);
	if(CmdId==PRT_BC_CUT_EJ) Sleep(1000); //IsackOK return false
	
	if(!IsAckOK())
		if(CmdId!=PRT_BC_CUT_EJ)
			return FALSE;
//MessageBox(NULL,"ack","",0);

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
//MessageBox(NULL,"3","1",0);

	if(!CmdOK) return FALSE;
//MessageBox(NULL,"4","1",0);
	if(CmdId==READ_BC_FIELD) HaveBarcode=TRUE; 

	return TRUE;
}


BOOL CEltra2017::Test()
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


BOOL CEltra2017::El_ReadBarcode(char *bar)
{
//	MessageBox(NULL,"chk read","",0);
	if(El_InsertPos(1))//2017
	if(SendPrintCmd(READ_BC_FIELD))
		if(HaveBarcode)
		{
			strcpy(bar,BarcodeNum);
			return TRUE;
		}
		return FALSE;
}

BOOL CEltra2017::OpenDev()
{

	HANDLE hCom;
	
	CString Portstr;
	Portstr.Format(_T("COM%d:"), m_nPortNo%10);
//	Portstr.Format("COM%d:", m_nPortNo%10);
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

BOOL CEltra2017::IsTktActive()//Eltra2017- 
{
#ifdef _DEBUG
	//	if(0)
	if(DemoEntry)
	{
		return TRUE;
	}
	return FALSE;
#endif
	if(!SendPrintCmd(STATUS)) return FALSE;
	//记录几种状态下的 参数
	CString tmp;
	tmp.Format("%d,%d,%d",RecData[2],RecData[3],RecData[4]);
//	MessageBox(NULL,tmp,"",0);
	if (RecData[3]==0x30) return TRUE;
	else return FALSE;
}

BOOL CEltra2017::IsTktActive(char *tkt)
{
	Sleep(20);
	char str[100];
	memset(str,'\0',100);
	DWORD wCount=16;//读取的字节数
	BOOL bReadStat;
	for(int i=0;i<3;i++)
	{
		Sleep(500);
		bReadStat=ReadFile(m_Handle,tkt,wCount,&wCount,NULL);
		tkt[wCount]=0;
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
/* old version
int CEltra2017::El_Status()
{
	// 0x30-NULL,32-mouth,36-inner,
	if(!SendPrintCmd(STATUS)) return -1;
	//	return RecData[1]-0x30;
	if (RecData[1]==0x30) return 0;
	else if (RecData[1]==0x32) return 2;
	else if (RecData[1]==0x36) return 6;
	else return 0;
}
*/
int CEltra2017::El_Status()
{
	// 0x30-NULL,32-mouth,36-inner,
	if(!SendPrintCmd(STATUS)) return -1;
	//	return RecData[1]-0x30;
	if (RecData[3]==0x31) return 0;
	else if (RecData[3]==0x30) return 2;// mouth
	else if (RecData[4]==0x30) return 6;//inner
	else return 0;
}


BOOL CEltra2017::El_Init()
{
	if(!SendPrintCmd(INIT)) return FALSE;

	return TRUE;
}

BOOL CEltra2017::El_Insert()
{
	if(!SendPrintCmd(INSERT_TKT)) return FALSE;
	return TRUE;
}

BOOL CEltra2017::El_Eject()
{
	if(!SendPrintCmd(EJECT_TKT)) return FALSE;
	return TRUE;
}

BOOL CEltra2017::El_Capture()
{
	if(!SendPrintCmd(CAP_TKT)) return FALSE;
	return TRUE;
}

BOOL CEltra2017::El_Print(char *Tkt_NO,char *Entertime)
{
	if(CombineTktData(Tkt_NO,Entertime))
		if(!SendPrintCmd(PRT_BC_CUT_EJ)) 
			return FALSE;
	return TRUE;
}

BOOL CEltra2017::El_PrintDiscountTkt(char *Tkt_NO,char *prndate,char * validtodate)
{
	if(CombineDisTktData(Tkt_NO,prndate,validtodate))
		if(!SendPrintCmd(PRT_BC_CUT_EJ)) 
			return FALSE;
		//		MessageBox(NULL,"9","1",0);
		return TRUE;
}
BOOL CEltra2017::El_PrintDiscountTkt(char *Tkt_NO,char *prndate,char * validtodate,int freehour)
{
	if(CombineDisTktData(Tkt_NO,prndate,validtodate,freehour))
		if(!SendPrintCmd(PRT_BC_CUT_EJ)) 
			return FALSE;
		//		MessageBox(NULL,"9","1",0);
		return TRUE;
}

BOOL CEltra2017::El_SetAddr()
{
	memcpy(SetAddr+4,BCTicket.Head1,strlen(BCTicket.Head1));
	LRC(SetAddr,strlen(SetAddr));

	if(!SendPrintCmd(SETADDR)) return FALSE;
	return TRUE;
}

BOOL CEltra2017::Cmd_Validate(char *pay)
{
	int CmdId=PRT_BC_CUT_EJ;
	char ValidHead[]={0x12,0x02,0x42,0x4c};
	char Valid[256];
	char ValidTail[]={0x03,0x30,0x30,0x04};
//	char CmdBuf[256];
	char *CmdBuf;

// 	char ValidateData1[]="000009A002D00005056789056";
// 	char ValidateData2[]="1FF006001B4010E2Nr.:";
// 	char ValidateData3[]="1FF004001B4010E2EXIT:";
// 	char ValidateData4[]="1FF002001B4010E2GATE:";
// 	//char ValidateData5[]="1FE006001F4010E256789056";//org- ok
// 	char ValidateData5[]="1FE006001F4010E2567890";
 	char ValidateData6[]="1FE004001B4010E212/12/2013";
// 	char ValidateData7[]="1FE002001F4010E2B1";


//	memset(Valid,128,0);
	int len;
	len=4;
	memcpy(Valid,ValidHead,len);
	
	memcpy(ValidateData1+17,pay+4,8);//barcode
// 	ValidateData1[17]='2';
// 	ValidateData1[18]='A';
	memcpy(Valid+len,ValidateData1,strlen(ValidateData1));	
	len+=strlen(ValidateData1);
	Valid[len++]=10;	
	Valid[len]=0;	
// 	memcpy(Valid+len,ValidateData2,strlen(ValidateData2));
// 	len+=strlen(ValidateData2);
// 	Valid[len++]=10;	
// 	Valid[len]=0;	
// 	memcpy(Valid+len,ValidateData3,strlen(ValidateData3));	
// 	len+=strlen(ValidateData3);
// 	Valid[len++]=10;	
// 	Valid[len]=0;	
// 	memcpy(Valid+len,ValidateData4,strlen(ValidateData4));	
// 	len+=strlen(ValidateData4);
// 	Valid[len++]=10;	
// 	Valid[len]=0;	
	memcpy(ValidateData5+15,pay+7,7);
// 	memcpy(Valid+len,ValidateData5,strlen(ValidateData5));	//number
// 	len+=strlen(ValidateData5);
// 	Valid[len++]=10;	
// 	Valid[len]=0;	
	memcpy(ValidateData6+16,pay,12);//datetime
	memcpy(Valid+len,ValidateData6,strlen(ValidateData6));	
	len+=strlen(ValidateData6);
	Valid[len++]=10;	
	Valid[len]=0;	
// 	memcpy(Valid+len,ValidateData7,strlen(ValidateData7));	
// 	len+=strlen(ValidateData7);
// 	Valid[len++]=10;	
// 	Valid[len]=0;	
	memcpy(Valid+len,ValidTail,4);	
	len+=4;
	Valid[len]=0;	


//	strcpy(CmdBuf,Valid);	
	CmdBuf=Valid;

//	LRC(CmdBuf,strlen(CmdBuf));
#ifdef _DEBUG
	return FALSE;
#endif
	dwBytesWrite=strlen(CmdBuf);
	ClearCommError(m_Handle,&dwErrorFlags,&ComStat);
	bWriteStat=WriteFile(m_Handle,CmdBuf,dwBytesWrite,& dwBytesWrite,NULL);
	Sleep(100); //delay1
	if(!bWriteStat) return FALSE;
	//MessageBox(NULL,"write","",0);
	if(CmdId==PRT_BC_CUT_EJ) Sleep(1000); //IsackOK return false
	
	if(!IsAckOK())
		if(CmdId!=PRT_BC_CUT_EJ)
			return FALSE;
		//MessageBox(NULL,"ack","",0);
		
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
				//MessageBox(NULL,"3","1",0);
	return TRUE;//El0000- test upload exit record.
				
				if(!CmdOK) return FALSE;
				//MessageBox(NULL,"4","1",0);
				
	return TRUE;
}

BOOL CEltra2017::Cmd_Park()
{
	int CmdId=0x70;
	//	unsigned char CmdBuf[]={0x12,0x02,0x42,'D',0x03,0,0x04,0,0,0,0,0,0,0,0};	
	char PrintBar2[]={0X12,0X02,0x42,0x4C,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x0A,
		0x4C,0x69,0x6E,0x65,0x20,0x54,0x65,0x78,0x74,0x20,0x31,0x0A,0x4C,0x69,0x6E,0x65,0x20,0x54,0x45,0x58,0x54,0x20,0x32,0x0A,
		0x4C,0x49,0x4E,0x45,0x20,0x54,0x45,0x58,0x54,0x20,0x33,0x0A,0x4C,0x49,0x4E,0x45,0x20,0x74,0x65,0x78,0x74,0x20,0x34,0x0A,0x03,0x3333,0x04};
	char InsertTkt[]={0x12,0x02,0x42,0x3C,0x03,0x363D,0x04,0};
	char ReadBarcode[]={0x12,0x02,0x42,0x43,0x03,0x3132,0x04,0};
	char Eject[]={0x12,0x02,0x42,0x46,0x03,0x3137,0x04,0};
	char InitDev[]={0x12,0x02,0x42,0x44,0x03,0x3135,0x04,0};	//init 12 02 42 44 03 3135 04
	char Status[]={0x12,0x02,0x42,0x3f,0x03,0x3135,0x04,0};	
	char Park[]={0x12,0x02,0x42,0x70,0x03,0x3030,0x04,0};	//2017- if 0x3030 should be 2 0x30
	char CmdBuf[256];
	
//	char CmdInsert[]={0x12,0x02,0x42,0x3c,0x30,0x31,0x30,0x30,0x0a,0x03,0x30,0x30,0x04,0};	//2017- new firmware
	char CmdInsert[]={0x12,0x02,0x42,0x3c,0x30,0x32,0x30,0x30,0x0a,0x03,0x30,0x30,0x04,0};	//2017- new firmware
	
	if(CmdId==0x70) strcpy(CmdBuf,Park);
	else
	if(CmdId==PRT_BC_CUT_EJ ||CmdId==PRT_TXT_CUT_EJ)
		strcpy(CmdBuf,BarTicket);	
	else if(CmdId==SETADDR)
		strcpy(CmdBuf,SetAddr);	
	else if(CmdId==INSERT_TKT)//2017- NEW FIRMEARE
		strcpy(CmdBuf,CmdInsert);
	else 
	{
		strcpy(CmdBuf,NormalCmd);
		CmdBuf[3]=CmdId;
	}
	LRC(CmdBuf,strlen(CmdBuf));
	
	dwBytesWrite=strlen(CmdBuf);
	ClearCommError(m_Handle,&dwErrorFlags,&ComStat);
	bWriteStat=WriteFile(m_Handle,CmdBuf,dwBytesWrite,& dwBytesWrite,NULL);
	Sleep(100); //delay1
	if(!bWriteStat) return FALSE;
	//MessageBox(NULL,"write","",0);
	if(CmdId==PRT_BC_CUT_EJ) Sleep(1000); //IsackOK return false

	if(CmdId==0x70) Sleep(2000); //IsackOK return false
	
	if(!IsAckOK())
		if(CmdId!=PRT_BC_CUT_EJ)
			return FALSE;
		//MessageBox(NULL,"ack","",0);
		
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
				//MessageBox(NULL,"3","1",0);
				
				if(!CmdOK) return FALSE;
				//MessageBox(NULL,"4","1",0);
				
	return TRUE;
}

BOOL CEltra2017::El_InsertPos(int p)
{
	int CmdId=0x3C;
	char CmdBuf[256];
	
	char CmdInsert[]= {0x12,0x02,0x42,0x3c,0x30,0x30,0x31,0x30,0x0a,0x03,0x30,0x30,0x04,0};	//2017- new firmware
	char CmdInsert2[]={0x12,0x02,0x42,0x3c,0x30,0x31,0x30,0x30,0x0a,0x03,0x30,0x30,0x04,0};	//2017-the second barcode read ok
	char CmdInsert3[]={0x12,0x02,0x42,0x3c,0x30,0x31,0x30,0x30,0x0a,0x03,0x30,0x30,0x04,0};	//2017-Try print the second barcode 
	
	if(p==1)
		strcpy(CmdBuf,CmdInsert);
	else if(p==2)
		strcpy(CmdBuf,CmdInsert2);
	else if(p==3)
		strcpy(CmdBuf,CmdInsert);

	LRC(CmdBuf,strlen(CmdBuf));
	
	dwBytesWrite=strlen(CmdBuf);
	ClearCommError(m_Handle,&dwErrorFlags,&ComStat);
	bWriteStat=WriteFile(m_Handle,CmdBuf,dwBytesWrite,& dwBytesWrite,NULL);
	Sleep(100); //delay1
	if(!bWriteStat) return FALSE;
	//MessageBox(NULL,"write","",0);
	
	if(!IsAckOK())
			return FALSE;
		//MessageBox(NULL,"ack","",0);
		
		Sleep(200);
		
		BOOL CmdOK = CheckCmdResult(CmdId);

			
			if(!CmdOK )
				//	if(!CmdOK && CmdId==PRT_BC_CUT_EJ)
				for (int i=0;i<2;i++)
				{
					Sleep(1000);
					CmdOK = CheckCmdResult(CmdId);
					if(CmdOK) break;
				}
				//MessageBox(NULL,"3","1",0);
				
				if(!CmdOK) return FALSE;
				//MessageBox(NULL,"4","1",0);
				
	return TRUE;
}

BOOL CEltra2017::ReadTicket(char *tkt)
{
#ifdef _DEBUG
	//bReadStat=1;
	//	wCount=16;
	//	if(0)
	if(DemoEntry)
	{
		sprintf(tkt,"B12345678");
		tkt[9]=0;
		DemoEntry=FALSE;
		return TRUE;
	}
	return FALSE;
#endif
	if(!El_InsertPos(1)) 
	{
		Sleep(6000);
		El_Init();
		return FALSE;
	}
	
	char tmp[20];
	
//	if(El_Status() == 3)
	if(El_Status() ==2)//2017
	{
		if(!El_ReadBarcode(tmp)) return FALSE;
	}
	else return FALSE;
	
	//	m_lpTktInfo->lpCreditInfo->m_fCreditAmt = RmnBal;
	//	m_lpTktInfo->lpszCAN = "B01000012";
// 	m_lpTktInfo->IsDiscountTicket=FALSE;
// 	m_lpTktInfo->FreeHours = 0;
	
	if(tmp[2]=='8')
	{
		return FALSE;// HA 1-Base function- disable discount ticket. 
		
		int curYear,curMonth,curDay;
		int vYear,vMonth,vDay;
		
		vYear=(tmp[9]-'0')*10+(tmp[10]-'0');
		vMonth=(tmp[11]-'0')*10+(tmp[12]-'0');
		vDay=(tmp[13]-'0')*10+(tmp[14]-'0');
		
		COleDateTime t=COleDateTime::GetCurrentTime();
		curYear = t.GetYear()%100;
		curMonth = t.GetMonth();
		curDay = t.GetDay();
		
		if(vYear>curYear || vYear==curYear && vMonth>curMonth || vYear==curYear && vMonth==curMonth && vDay>=curDay)
		{
// 			m_lpTktInfo->IsDiscountTicket=TRUE;
// 			m_lpTktInfo->FreeHours = 6; //UCH 3- test
			El_Capture();
			tmp[0]='F';
			tmp[1]='0';
			tmp[2]='3';
			tmp[9]=0;
			m_lpTktInfo->lpszCAN = tmp;
		}
		else
			return FALSE;
	}
	else if(tmp[2]=='0')
	{
		tmp[2]='B';
		tmp[11]=0;
		sprintf(tkt,"%s",tmp+2);

		CString buf;
		buf.Format("%s",tkt);
//		MessageBox(NULL,buf,"r3",0);
	}
	else
		return FALSE; //UCH 3- invalid ticket if it isn't hourly and discount ticket

	return TRUE;
}
