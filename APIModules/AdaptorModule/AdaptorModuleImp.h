// AdaptorModuleImp.h: interface for the AdaptorModuleImp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AdaptorModuleIMP_H__29B3858A_4BDE_46B6_BDEC_EFBAF1DAF42C__INCLUDED_)
#define AFX_AdaptorModuleIMP_H__29B3858A_4BDE_46B6_BDEC_EFBAF1DAF42C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ModuleAdaptor.h"

class ResizeableStringFormatter;
class StrPtrLen; 

typedef boost::shared_ptr<ModuleAdaptor> LP_S_ModuleAdaptor;

class AdaptorModuleImp
{
public:
	AdaptorModuleImp();
	virtual ~AdaptorModuleImp();

	// Module dispatches
	///////////////////////     Global ROLES    ///////////////////////////////
	virtual QTSS_Error	Register(QTSS_Register_Params* inParams);
	virtual QTSS_Error	Initialize(QTSS_Initialize_Params* inParams);
	virtual QTSS_Error	Shutdown();
		//	virtual QTSS_Error	RereadPrefs()  ;								//.
//	///////////////////////     RTSP-specific  ///////////////////////////////
		//	virtual QTSS_Error	RTSP_Filter(QTSS_StandardRTSP_Params* inParams)  ;
		//	virtual QTSS_Error	RTSP_Route(QTSS_StandardRTSP_Params* inParams)  ;
		//	virtual QTSS_Error	RTSP_Authenticate(QTSS_RTSPAuth_Params* inParams)  ;
		//	virtual QTSS_Error	RTSP_Authorize(QTSS_StandardRTSP_Params* inParams)  ;
			virtual QTSS_Error	RTSP_ProcessPreRTSPRequest(QTSS_StandardRTSP_Params* inParams)  ;	//Implemented
		//	virtual QTSS_Error	RTSP_ProcessRTSPRequest(QTSS_StandardRTSP_Params* inParams)  ;
		//	virtual QTSS_Error	RTSP_PostProcess(QTSS_StandardRTSP_Params* inParams)  ;
		//	virtual QTSS_Error	RTSP_SessionClosing(QTSS_RTSPSession_Params* inParams)  ;
//	///////////////////////     RTP-specific  ///////////////////////////////
			virtual QTSS_Error	RTP_SendPackets(QTSS_RTPSendPackets_Params* inParams)  ;
			virtual QTSS_Error	RTP_ClientSessionClosing(QTSS_ClientSessionClosing_Params* inParams)  ;
		//	virtual QTSS_Error	RTCP_Process(QTSS_RTCPProcess_Params* inParams)  ;
//	///////////////////////     File system roles  ///////////////////////////////
		//	virtual QTSS_Error   File_OpenFile(QTSS_OpenFile_Params* inParams)  ;
		//	virtual QTSS_Error   File_AdviseFile(QTSS_AdviseFile_Params* inParams)  ;
		//	virtual QTSS_Error   File_ReadFile(QTSS_ReadFile_Params* inParams)  ;
		//	virtual QTSS_Error   File_CloseFile(QTSS_CloseFile_Params* inParams)  ;
		//	virtual QTSS_Error   File_RequestEventFile(QTSS_RequestEventFile_Params* inParams)  ;
//
//	///////////////////////     Specic RTSP Process  ///////////////////////////////
	virtual QTSS_Error DoDescribe(QTSS_StandardRTSP_Params* inParams)  ;
	virtual QTSS_Error DoSetup(QTSS_StandardRTSP_Params* inParams)  ;
	virtual QTSS_Error DoPlay(QTSS_StandardRTSP_Params* inParams)  ;
	virtual QTSS_Error DoPause(QTSS_StandardRTSP_Params* inParams)  ;		//该方法暂时没用起来
	virtual QTSS_Error DoTearDown(QTSS_StandardRTSP_Params* inParams)  ;	//该方法暂时没用起来

	static AdaptorModuleImp* GetInstance();

protected:
	static AdaptorModuleImp*  fModuleImp ;		//For Singleton model
	
	ModuleAdaptor fModuleAdaptor;

private:
	UInt32 WriteSDPHeader(/*FILE* sdpFile, */iovec *theSDPVec, SInt16 *ioVectorIndex, StrPtrLen *sdpHeader);
	Bool16 isRequestSelfDefined(QTSS_StandardRTSP_Params* inParams);
	QTSS_Error genDoDescribeSDP_ForDoDescribe(ResizeableStringFormatter* inResizeableStringFormatter, QTSS_StandardRTSP_Params* inParams);

	enum
	{
			ALL_ROLE_ENABLED = 0				//是否打开模块的所有角色, 如果打开所有角色，如认证角色，会需要输入用户名和密码
	};


};

#endif // !defined(AFX_AdaptorModuleIMP_H__29B3858A_4BDE_46B6_BDEC_EFBAF1DAF42C__INCLUDED_)
