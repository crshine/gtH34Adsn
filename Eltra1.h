#if !defined(AFX_ELTRA_H__D4BB750F_A782_492F_8DEA_9D2C6DD1BFEE__INCLUDED_)
#define AFX_ELTRA_H__D4BB750F_A782_492F_8DEA_9D2C6DD1BFEE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Eltra.h : header file
//
//#include "ticketinfo.h"

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

struct BCTICKET
{
	char Barcode[16];
	char Head1[20];
	char Head2[20];
	char Head3[20];
	char Line1[20];
	char Line2[20];
	char Line3[20];
	char Line4[20];
};



/////////////////////////////////////////////////////////////////////////////
// CEltra document

class CEltra : public CObject
{

public:
	CEltra(int Comport);
	virtual ~CEltra();

//	LPTICKETINFO lpTktInfo;
//	LPTICKETINFO m_lpTktInfo;
private:
	int m_nPortNo;
	HANDLE m_Handle;
	int m_nSysID;

	BOOL HaveBarcode;
	char RecData[128];
	char BarcodeNum[20];

	COMSTAT ComStat;
	DWORD dwErrorFlags;
	BOOL bWriteStat;
	DWORD dwBytesWrite;

	BCTICKET BCTicket;

public:
	BOOL OpenDev();
	BOOL IsTktActive();
	BOOL El_ReadBarcode(char *bar);
//	BOOL ReadTicket(LPTICKETINFO * ppTktInfo);
//	BOOL IssueTicket(CString CAN, int VehType, int StnID, LPTICKETINFO * ppTicket);
	BOOL El_Init();
	int  El_Status();
	BOOL El_Insert();
	BOOL El_Eject();
	BOOL El_Capture();
	BOOL El_Print(char *, char *);
	BOOL El_PrintDiscountTkt(char *Tkt_NO,char *Prntime,int freehours);
	BOOL El_SetAddr();

	BOOL Test();
	BOOL Send_RMPM_Cmd(int port,BOOL IsON);

private:

	BOOL sendtocomm(LPSTR sendbuf, int length);

	BOOL IsAckOK();
	BOOL AckData();
	BOOL SendInquiryCmd();
	BOOL SendPrintCmd(int CmdId);
	BOOL CheckCmdResult(int CmdId);

	BOOL CombineTktData(char *, char *);
	BOOL CombineDisTktData(char *ticketNo, char *prntime,int hours);
	void LRC(char *buf,int len);

	// Generated message map functions
protected:
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ELTRA_H__D4BB750F_A782_492F_8DEA_9D2C6DD1BFEE__INCLUDED_)
