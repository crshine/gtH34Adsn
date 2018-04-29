#ifndef _LOADDLL_H_
#define _LOADDLL_H_

#define LIBPATH "..\\Common\\"

//		note!
//				you must replace
//				"...\\SDS\\" like "d:\\ccc\\Cashier\\" or "g:\\work\\SDS\\"
//				to current path of the workspace in
//				"...\\SDS\\"



#ifdef _ADD_OTP_DLL_
	#include "..\\Common\\OTP\\rwl_exp.h"
	#pragma comment(lib, "..\\common\\OTP\\rwl.lib")
//	#pragma message("Automatic link the rwl.lib")
	#undef _ADD_OTP_DLL_
#endif

#ifdef  _ADD_LOG_DLL_
	#include "..\\Common\\EVENTLOG\\Log.h"
#ifdef _DEBUG
	#pragma comment(lib,"..\\Common\\EVENTLOG\\Debug\\EventLog.lib") 
//	#pragma message("Automatically linking EventLog.lib(Debug Version)")
#else
	#pragma comment(lib,"..\\Common\\EVENTLOG\\release\\EventLog.lib") 
//	#pragma message("Automatically linking EventLog.lib(Release Version)")
#endif
	#undef _ADD_LOG_DLL_
#endif


#ifdef  _ADD_XFILES_DLL_
	#include "..\\FileManage\\XFiles\\XFiles.h"
#ifdef _DEBUG
	#pragma comment(lib,"..\\FileManage\\XFiles\\Debug\\XFiles.lib") 
//	#pragma message("Automatically linking XFiles.lib(Debug Version)")
#else
	#pragma comment(lib,"..\\FileManage\\XFiles\\release\\XFiles.lib") 
//	#pragma message("Automatically linking XFiles.lib(Release Version)")
#endif
	#undef _ADD_XFILES_DLL_
#endif

#ifdef _ADD_RAS_SUPPORT_
	#include "ras.h"
	#pragma comment(lib, "Rasapi32.lib") 
//	#pragma message("Automatically linking Rasapi32.lib")
#endif 

#ifdef _ADD_WININET_SUPPORT_
	#include "..\\common\\Dialup\\wininet.h"
	#pragma comment(lib, "..\\common\\Dialup\\wininet.lib")
//	#pragma message("Automatically linking wininet.lib")
#endif

#ifdef _ADD_UTILITIES_DLL_
//	#include "..\\Common\\Utility\\utilities.h"
	#include "..\\Common\\Utility\\utilities.h"
#ifdef _DEBUG
	#pragma comment(lib, "..\\Common\\Utility\\debug\\utilities.lib") 
#else
	#pragma comment(lib,"..\\Common\\Utility\\release\\utilities.lib") 
#endif
	#undef _ADD_UTILITIES_DLL_
#endif

#ifdef _ADD_REGISTRY_DLL_
//	#include "d:\\ccc\\Cashier\\Common\\registry\\registry.h"
	#include "..\\Common\\registry\\registry.h"
#ifdef _DEBUG
	#pragma comment(lib, "..\\Common\\registry\\debug\\registry.lib") 
//	#pragma message("Automatically linking registry.lib(Debug Version)")
#else
	#pragma comment(lib,"..\\Common\\registry\\release\\registry.lib") 
//	#pragma message("Automatically linking registry.lib(Release Version)") 
#endif
#endif


#ifdef _ADD_DISCOUNT_DLL_
	#include "..\\tryrule\\discount.h"
#ifdef _DEBUG
	#pragma comment(lib,"..\\tryrule\\debug\\discount.lib") 
//	#pragma message("Automatically linking discount.lib(Debug Version)")
#else
	#pragma comment(lib,"..\\tryrule\\release\\discount.lib") 
//	#pragma message("Automatically linking discount.lib(Release Version)") 
#endif
#endif

#ifdef _ADD_CHARGINGRULE_DLL_
	#include "..\\tryrule\\chargingrule.h"
#ifdef _DEBUG
	#pragma comment(lib,"..\\tryrule\\debug\\chargingrule.lib") 
//	#pragma message("Automatically linking chargingrule.lib(Debug Version)")
#else
	#pragma comment(lib,"..\\tryrule\\release\\chargingrule.lib") 
//	#pragma message("Automatically linking chargingrule.lib(Release Version)") 
#endif
#endif

#ifdef _ADD_CATEGORY_DLL_
	#include "..\\tryrule\\category.h"
#ifdef _DEBUG
	#pragma comment(lib,"..\\tryrule\\debug\\category.lib") 
//	#pragma message("Automatically linking category.lib(Debug Version)")
#else
	#pragma comment(lib,"..\\tryrule\\release\\category.lib") 
//	#pragma message("Automatically linking category.lib(Release Version)") 
#endif
#endif

#ifdef _ADD_STATIONS_DLL_
	#include "..\\tryrule\\stations.h"
#ifdef _DEBUG
	#pragma comment(lib,"..\\tryrule\\debug\\stations.lib") 
//	#pragma message("Automatically linking stations.lib(Debug Version)")
#else
	#pragma comment(lib,"..\\tryrule\\release\\stations.lib") 
//	#pragma message("Automatically linking stations.lib(Release Version)") 
#endif
#endif

#ifdef _ADD_SYSSETTINGS_DLL_
	#include "..\\tryrule\\syssettings.h"
#ifdef _DEBUG
	#pragma comment(lib,"..\\tryrule\\debug\\syssettings.lib") 
//	#pragma message("Automatically linking syssettings.lib(Debug Version)")
#else
	#pragma comment(lib,"..\\tryrule\\release\\syssettings.lib") 
//	#pragma message("Automatically linking syssettings.lib(Release Version)") 
#endif
#endif

#ifdef _ADD_DISCOUNTPLAN_DLL_
	#include "..\\tryrule\\discountplan.h"
#ifdef _DEBUG
	#pragma comment(lib,"..\\tryrule\\debug\\discountplan.lib") 
//	#pragma message("Automatically linking discountplan.lib(Debug Version)")
#else
	#pragma comment(lib,"..\\tryrule\\release\\discountplan.lib") 
//	#pragma message("Automatically linking discountplan.lib(Release Version)") 
#endif
#endif

#ifdef _ADD_TRANSACTION_DLL_
	#include "..\\tryrule\\transaction.h"
#ifdef _DEBUG
	#pragma comment(lib,"..\\tryrule\\debug\\transaction.lib") 
//	#pragma message("Automatically linking transaction.lib(Debug Version)")
#else
	#pragma comment(lib,"..\\tryrule\\release\\transaction.lib") 
//	#pragma message("Automatically linking transaction.lib(Release Version)") 
#endif
#endif

#ifdef  _ADD_RECEIPTPRN_DLL_
	#include "..\\Common\\PC_ReceiptPrn\\RECEIPTPRN.h"
#ifdef _DEBUG
	#pragma comment(lib,"..\\Common\\PC_ReceiptPrn\\Debug\\RECEIPTPRN.lib") 
//	#pragma message("Automatically linking EventLog.lib(Debug Version)")
#else
	#pragma comment(lib,"..\\Common\\PC_ReceiptPrn\\release\\RECEIPTPRN.lib") 
//	#pragma message("Automatically linking EventLog.lib(Release Version)")
#endif
	#undef _ADD_LOG_DLL_
#endif

#endif
 