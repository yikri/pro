// QTSSLogging.cpp: implementation of the QTSSLogging class.
//
//////////////////////////////////////////////////////////////////////

#include "OSHeaders.h"
#include "StrPtrLen.h"
#include "OSArrayObjectDeleter.h"
#include "QTSSModuleUtils.h"
#include "OSMemory.h"
#include "ev.h"
#include "QTSSMemoryDeleter.h"
//Used for the definitions
#include "EnumStringMapping.h"

#include "QTSSLogging.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//The sequence define here should be same with qtssDescribeMethod, 
static char* MethodSeries[] = 
{
    "qtssDescribeMethod", 
	"qtssSetupMethod",
	"qtssTeardownMethod",
	"qtssNumVIPMethods",
	
	"qtssPlayMethod",
	"qtssPauseMethod",
	"qtssOptionsMethod",
	"qtssAnnounceMethod",
	"qtssGetParameterMethod",
	"qtssSetParameterMethod",
	"qtssRedirectMethod",
	"qtssRecordMethod",
	
	"qtssNumMethods",
	"qtssIllegalMethod"		
};


QTSSLogging::ST_QTSSLogging QTSSLogging::fModulesDetail[] = 
{
	{"AdaptorModule", QTSS_RTSPPreProcessor_Role}
};

QTSSLogging::QTSSLogging()
{
	;
}

QTSSLogging::~QTSSLogging()
{
	;
}



/************************************************************************/
//  ModuleLog
//		to Enable the log, you should add this funciton to be called "QTSSModule.h"
//			QTSS_Error  CallDispatch(QTSS_Role inRole, QTSS_RoleParamPtr inParams)
//
/************************************************************************/

QTSS_Error QTSSLogging::ModuleDispatchLog(QTSS_ModuleObject inModule, QTSS_Role inRole, QTSS_RoleParamPtr inParamBlock)
{

// 
//1. get Module Name
	char theNameStr[256];
	if (inModule)
	{
		StrPtrLen theName;
		QTSS_Error theErr = QTSS_GetValuePtr(inModule, qtssModName, 0, (void**)&theName.Ptr, &theName.Len);
		theErr;
		memset(theNameStr, 0, 256*sizeof(char));
		memcpy (theNameStr, theName.Ptr, theName.Len);
	}
//
//2. print Module callation

	//2.1 Get the index of the array
	if(LOGGING_Print_Module_Called) 
	{
		int theFindedIndex = -1;
		for (int i = 0; i < sRole_RoleName_Map_Size; i++)
		{
			if (Role_RoleName_Map[i].fRoleValue == inRole)
			{
				theFindedIndex = i;
				break;
			}
		}
		//
		if(theFindedIndex > -1)
		{
			if (inModule)
			{
				qtss_printf("		%-32s		called,		%-40s\n", Role_RoleName_Map[theFindedIndex].fRoleName, theNameStr);
			}
			else
			{
				qtss_printf("		%-32s		called\n", Role_RoleName_Map[theFindedIndex].fRoleName);
			}
		}
	}
//3. Print Callation detailes
	//
	if (
		LOGGING_Print_Module_Details_ALL ||
		isModule_Called_Details_Enabled(theNameStr, inRole)
		)
	{
		ModuleDispatchDetailLog(inModule, inRole, inParamBlock);
	}
	return QTSS_NoErr;
}


QTSS_Error QTSSLogging::ModuleDispatchDetailLog(QTSS_ModuleObject /*inModule*/, QTSS_Role inRole, QTSS_RoleParamPtr inParamBlock)
{
    switch (inRole)		
    {
		/////////////////////////////////////////////////////////////////////////////////////////////
		///////////////////////     Global ROLES    ///////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////
		
	case QTSS_Register_Role:	//return pModuleImp->Register(&inParamBlock->regParams);
	case QTSS_Initialize_Role:	//return pModuleImp->Initialize(&inParamBlock->initParams);		//Imp, refer to QTSS_Initialize_Params initParams;
	case QTSS_Shutdown_Role:	//NULL parameters
	case QTSS_ErrorLog_Role:	//return LogError(inParamBlock);
	case QTSS_RereadPrefs_Role:	//NULL parameters
	case QTSS_StateChange_Role:	//return StateChange(&inParamBlock->stateChangeParams);
	case QTSS_Interval_Role:	//NULL
		break;		
		/////////////////////////////////////////////////////////////////////////////////////////////
		///////////////////////     RTSP-specific  ///////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////
		
	case QTSS_RTSPFilter_Role:	//return pModuleImp->RTSP_Filter(&inParamBlock->rtspRequestParams);	//refer to QTSS_Filter_Params rtspFilterParams;
	case QTSS_RTSPRoute_Role:	//return pModuleImp->RTSP_Route(&inParamBlock->rtspRouteParams);		//refer to 
	case QTSS_RTSPAuthenticate_Role:	//return pModuleImp->RTSP_Authenticate(&inParamBlock->rtspAthnParams);//refer to 
	case QTSS_RTSPAuthorize_Role:		//return RTSP_Authenticate(&inParamBlock->rtspRequestParams);		//refer to 
		break;
		
	case QTSS_RTSPPreProcessor_Role:	//return pModuleImp->RTSP_ProcessPreRTSPRequest(&inParamBlock->rtspRequestParams);	//Imp, refer to 
		{
			//1. Get the method
			QTSS_RTSPMethod* theMethod = NULL;
			UInt32 theMethodLen = 0;
			QTSS_StandardRTSP_Params* theParamBlock = (QTSS_StandardRTSP_Params*)inParamBlock;
			//
			if ((QTSS_GetValuePtr(theParamBlock->inRTSPRequest, qtssRTSPReqMethod, 0,
				(void**)&theMethod, &theMethodLen) != QTSS_NoErr) || (theMethodLen != sizeof(QTSS_RTSPMethod)))
			{
				Assert(0);			return QTSS_RequestFailed;
			}//qtss_printf("		Methods: %-32s		called,		%-40s\n", Role_RoleName_Map[i].fRoleName, theNameStr);

			//2. Print the method with parameters: QTSS_RoleParamPtr  :	QTSS_StandardRTSP_Params
			//qtss_printf("		Methods: %-32s		called,		%-40s\n", Role_RoleName_Map[i].fRoleName, theNameStr);
		}

	case QTSS_RTSPRequest_Role:			//return pModuleImp->RTSP_ProcessRTSPRequest(&inParamBlock->rtspRequestParams);		//refer to 
	case QTSS_RTSPPostProcessor_Role:	//return pModuleImp->RTSP_PostProcess(&inParamBlock->rtspPostProcessorParams);		//refer to 
	case QTSS_RTSPSessionClosing_Role:	//return pModuleImp->RTSP_SessionClosing(&inParamBlock->rtspSessionClosingParams);	//refer to 
	case QTSS_RTSPIncomingData_Role:	//return pModuleImp->ProcessRTPData(&inParamBlock->rtspIncomingDataParams);
		break;
		
		/////////////////////////////////////////////////////////////////////////////////////////////
		///////////////////////     RTP-specific  ///////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////
	case QTSS_RTPSendPackets_Role:		//return pModuleImp->RTP_SendPackets(&inParamBlock->rtpSendPacketsParams);			//Need Imp, refer to 
	case QTSS_ClientSessionClosing_Role://return pModuleImp->RTP_ClientSessionClosing(&inParamBlock->clientSessionClosingParams);	//refer to 
		/////////////////////////////////////////////////////////////////////////////////////////////
		///////////////////////     RTCP-specific  ///////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////
	case QTSS_RTCPProcess_Role:			//return pModuleImp->RTCP_Process(&inParamBlock->rtcpProcessParams);
	/////////////////////////////////////////////////////////////////////////////////////////////
		///////////////////////     File system roles  ///////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////
	case QTSS_OpenFilePreProcess_Role:	//break;		
	case QTSS_OpenFile_Role:			//return pModuleImp->File_OpenFile(&inParamBlock->openFileParams);
	case QTSS_AdviseFile_Role:			//return pModuleImp->File_AdviseFile(&inParamBlock->adviseFileParams);
	case QTSS_ReadFile_Role:			//return pModuleImp->File_ReadFile(&inParamBlock->readFileParams);
	case QTSS_CloseFile_Role:			//return pModuleImp->File_CloseFile(&inParamBlock->closeFileParams);
	case QTSS_RequestEventFile_Role:	//return pModuleImp->File_RequestEventFile(&inParamBlock->reqEventFileParams);
		break;
    };
	return QTSS_NoErr;
};


/************************************************************************/
//
//  RTSPSessionLog
//
/************************************************************************/

//
//Print RTSP Session Related Trace
//
QTSS_Error QTSSLogging::RTSPSessionRunLog(void* inRTSPSession, UInt32 inState, void* /*Obj1*/, void* /*Obj2*/)
{
	//
	//Get the index of the array
	//
	int theFindedIndex = -1;
	for (int i = 0; i < sRole_RoleName_Map_Size; i++)
	{
		if (RTSP_State_StateName_Map[i].fRoleValue == inState)
		{
			theFindedIndex = i;
			break;
		}
	}
	//
	if(theFindedIndex > -1)
	{
		qtss_printf("	%-40s	RTSPSession:	%lu\n", RTSP_State_StateName_Map[theFindedIndex].fRoleName, inRTSPSession);
	}
	return QTSS_NoErr;
}




/************************************************************************/
//
//  Private functions
//
/************************************************************************/
int QTSSLogging::isModule_Called_Details_Enabled(char* inModuleName, QTSS_Role inRole)
{
	for (int i = 0; i<LOGGING_Print_Module_Details_Nums; i++)
	{
		if( 
			(	inRole == fModulesDetail[i].fRole						) && 
			(	strcmp(fModulesDetail[i].fModName, inModuleName) == 0	)
			)
		{
			return 1;
		}
	}
	return 0;
}



void QTSSLogging::PrintClientSession(void* inpSession, char* inChar)
{
	qtss_printf("\nThe Client Session is: %lx,  %s is\n\n", inpSession, inChar);
	return;
}
