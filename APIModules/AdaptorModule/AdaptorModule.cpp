/*
	File:       AdaptorModule.cpp

	该文件从QTSSRawFileModule.cpp修改而来
*/


#include "AdaptorModule.h"
#include "QTSSLogging.h" 
#include "AdaptorModuleImp.h"


/************************************************************************/
// Description:	The Main Procedure for this module
// Parameters:	  inPrivateArgs: 
/************************************************************************/
static QTSS_Error AdaptorModuleDispatch(QTSS_Role inRole, QTSS_RoleParamPtr inParams);

#ifdef _DEBUG
QTSS_Error server_AdaptorModuleD_Main(void* inPrivateArgs)
#else
QTSS_Error server_AdaptorModule_Main(void* inPrivateArgs)
#endif
{
    return _stublibrary_main(inPrivateArgs, AdaptorModuleDispatch);	
}


/************************************************************************/
// Description:	The dispatch procedure for this module
/************************************************************************/
QTSS_Error AdaptorModuleDispatch(QTSS_Role inRole, QTSS_RoleParamPtr inParamBlock)
{
	AdaptorModuleImp* pModuleImp = AdaptorModuleImp::GetInstance();
	QTSS_Error theError = QTSS_NoErr;

    switch (inRole)		
    {
		/////////////////////////////////////////////////////////////////////////////////////////////
		///////////////////////     Global ROLES    ///////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////

        case QTSS_Register_Role:
            theError = pModuleImp->Register(&inParamBlock->regParams);			//Implemented
			break;

        case QTSS_Initialize_Role:
            theError = pModuleImp->Initialize(&inParamBlock->initParams);		//Implemented
			break;

		case QTSS_Shutdown_Role:
			theError = pModuleImp->Shutdown();								//Implemented
			break;
			
		case QTSS_ErrorLog_Role:
			//theError = LogError(inParamBlock);
			return theError;
			break;

        case QTSS_RereadPrefs_Role:
			//theError = pModuleImp->RereadPrefs();
			return theError;
			break;

		case QTSS_StateChange_Role:		//??
			//theError = StateChange(&inParamBlock->stateChangeParams);
			return theError;
			break;

        case QTSS_Interval_Role:		//??
			//theError = IntervalRole();
			return theError;
			break;

		/////////////////////////////////////////////////////////////////////////////////////////////
		///////////////////////     RTSP-specific  ///////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////

		case QTSS_RTSPFilter_Role:
			//theError = pModuleImp->RTSP_Filter(&inParamBlock->rtspRequestParams);
			return theError;
			break;

		case QTSS_RTSPRoute_Role:
			//theError = pModuleImp->RTSP_Route(&inParamBlock->rtspRouteParams);
			return theError;
			break;

		case QTSS_RTSPAuthenticate_Role:
			//theError = pModuleImp->RTSP_Authenticate(&inParamBlock->rtspAthnParams);
			return theError;
			break;

		case QTSS_RTSPAuthorize_Role:
			//theError = RTSP_Authenticate(&inParamBlock->rtspRequestParams);
			return theError;
			break;

		case QTSS_RTSPPreProcessor_Role:
            theError = pModuleImp->RTSP_ProcessPreRTSPRequest(&inParamBlock->rtspRequestParams);			//Implemented
			break;                               

		case QTSS_RTSPRequest_Role: 
            //theError = pModuleImp->RTSP_ProcessRTSPRequest(&inParamBlock->rtspRequestParams);	
			return theError;
			break;

		case QTSS_RTSPPostProcessor_Role:
			//theError = pModuleImp->RTSP_PostProcess(&inParamBlock->rtspPostProcessorParams);
			return theError;
			break;
			
		case QTSS_RTSPSessionClosing_Role:
			//theError = pModuleImp->RTSP_SessionClosing(&inParamBlock->rtspSessionClosingParams);			//Implemented
			return theError;
			break;
			
		case QTSS_RTSPIncomingData_Role:
			//theError = pModuleImp->ProcessRTPData(&inParamBlock->rtspIncomingDataParams);
			return theError;
			break;

		/////////////////////////////////////////////////////////////////////////////////////////////
		///////////////////////     RTP-specific  ///////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////

		case QTSS_RTPSendPackets_Role:
			theError = pModuleImp->RTP_SendPackets(&inParamBlock->rtpSendPacketsParams);					//Implmented
			break;

		case QTSS_ClientSessionClosing_Role:
			theError = pModuleImp->RTP_ClientSessionClosing(&inParamBlock->clientSessionClosingParams);		//Implemented
			break;

		/////////////////////////////////////////////////////////////////////////////////////////////
		///////////////////////     RTCP-specific  ///////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////

		case QTSS_RTCPProcess_Role:
			//theError = pModuleImp->RTCP_Process(&inParamBlock->rtcpProcessParams);
			return theError;
			break;

		/////////////////////////////////////////////////////////////////////////////////////////////
		///////////////////////     File system roles  ///////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////

		case QTSS_OpenFilePreProcess_Role:
			return theError;
			break;

		case QTSS_OpenFile_Role:
			//theError = pModuleImp->File_OpenFile(&inParamBlock->openFileParams);
			return theError;
			break;

		case QTSS_AdviseFile_Role:
			//theError = pModuleImp->File_AdviseFile(&inParamBlock->adviseFileParams);
			return theError;
			break;
			
		case QTSS_ReadFile_Role:
			//theError = pModuleImp->File_ReadFile(&inParamBlock->readFileParams);
			return theError;
			break;
			
		case QTSS_CloseFile_Role:
			//theError = pModuleImp->File_CloseFile(&inParamBlock->closeFileParams);
			return theError;
			break;

		case QTSS_RequestEventFile_Role:
			//theError = pModuleImp->File_RequestEventFile(&inParamBlock->reqEventFileParams);
			return theError;
			break;
    }

	QTSSLogging::ModuleDispatchLog(NULL, inRole, inParamBlock);

    return theError;
}

