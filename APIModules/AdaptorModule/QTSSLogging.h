// QTSSLogging.h: interface for the QTSSLogging class.
// by wuzengde, Oct.1, 2007
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QTSSLogging_H__BCC6D989_4468_45EA_B968_D5FA790F04C9__INCLUDED_)
#define AFX_QTSSLogging_H__BCC6D989_4468_45EA_B968_D5FA790F04C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


const int LOGGING_Print_Module = 0;

enum
{
	LOGGING_Print_Module_Called      = LOGGING_Print_Module,	// print which Module is called，Apply to all module
	LOGGING_Print_Module_Details_ALL = LOGGING_Print_Module,	// print which the Module parameter and detailed value
	LOGGING_Print_Module_Details_Nums = LOGGING_Print_Module,	// print which Module is called, only apply to specific module defined in "fModulesDetail"
};


class QTSSLogging
{
public:
	QTSSLogging();
	virtual ~QTSSLogging();
	//
	// Log which module are called with what role
	//
	static QTSS_Error ModuleDispatchLog(QTSS_ModuleObject inModule, QTSS_Role inRole, QTSS_RoleParamPtr inParamBlock);		//, ModuleDispatchType inType = ModuleDispatch_Module
	//
	// Log the dispatch details
	//
	static QTSS_Error ModuleDispatchDetailLog(QTSS_ModuleObject inModule, QTSS_Role inRole, QTSS_RoleParamPtr inParamBlock);
	//static QTSS_Error ModuleDispatchDetailLog_RTSP(QTSS_ModuleObject inModule, QTSS_Role inRole, QTSS_RoleParamPtr inParamBlock);
	//
	//
	//
	static QTSS_Error RTSPSessionRunLog(void* inRTSPSession, UInt32 inState, void* Obj1 = NULL, void* Obj2 = NULL );

	static void PrintClientSession(void* inpSession, char* inChar);	//用于跟踪客户端会话情况

public:
	struct ST_QTSSLogging
	{
		char* fModName;
		QTSS_Role fRole;
	};

private:
	static int isModule_Called_Details_Enabled(char* inModuleName, QTSS_Role inRole);

	static ST_QTSSLogging fModulesDetail[];
	
};

#endif // !defined(AFX_QTSSLogging_H__BCC6D989_4468_45EA_B968_D5FA790F04C9__INCLUDED_)
