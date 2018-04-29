#if !defined(AFX_ELTRA_H__D4BB750F_A782_492F_8DEA_9D2C6DD1BFEE__INCLUDED_)
#define AFX_ELTRA_H__D4BB750F_A782_492F_8DEA_9D2C6DD1BFEE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Eltra.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEltra document

class CEltra 
{

public:
	CEltra(int Comport);
	virtual ~CEltra();
private:
	int m_nPortNo;
	HANDLE m_Handle;

	COMSTAT ComStat;
	DWORD dwErrorFlags;
	BOOL bWriteStat;
	DWORD dwBytesWrite;


public:
	BOOL OpenDev();
	BOOL CloseDev();
	BOOL Send_RMPM_Cmd(int port,BOOL IsON);

private:


	// Generated message map functions
protected:
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ELTRA_H__D4BB750F_A782_492F_8DEA_9D2C6DD1BFEE__INCLUDED_)
