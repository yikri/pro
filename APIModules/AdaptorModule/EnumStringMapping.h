// EnumStringMapping.h
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EnumStringMapping_H__29B3858A_4BDE_46B6_BDEC_EFBAF1DAF42C__INCLUDED_)
#define AFX_EnumStringMapping_H__29B3858A_4BDE_46B6_BDEC_EFBAF1DAF42C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000




/************************************************************************/
/************************************************************************/
//
//  ModuleLog
//
/************************************************************************/
/************************************************************************/

struct ST_Enum_Str_Map
{
	UInt32 fRoleValue;
	char* fRoleName;
	void* fReserved;
};


ST_Enum_Str_Map Role_RoleName_Map[] = 
{
	//Global
	{  QTSS_Register_Role				,    	"QTSS_Register_Role", 			NULL },		//0
	{  QTSS_Initialize_Role				,    	"QTSS_Initialize_Role", 		NULL },		//1
	{  QTSS_Shutdown_Role				,    	"QTSS_Shutdown_Role", 			NULL },		//2
					
	{  QTSS_ErrorLog_Role				,    	"QTSS_ErrorLog_Role", 			NULL },		//3
	{  QTSS_RereadPrefs_Role			,    	"QTSS_RereadPrefs_Role",		NULL },		//4
	{  QTSS_StateChange_Role			,    	"QTSS_StateChange_Role",		NULL },		//5
					
	{  QTSS_Interval_Role				,    	"QTSS_Interval_Role", 			NULL },		//6
					
		//RTSP-specific
	{  QTSS_RTSPFilter_Role				,    	"QTSS_RTSPFilter_Role",			NULL },		//7
	{  QTSS_RTSPRoute_Role				,    	"QTSS_RTSPRoute_Role",			NULL },		//8
	{  QTSS_RTSPAuthenticate_Role		,    	"QTSS_RTSPAuthenticate_Role", 	NULL },	//9
	{  QTSS_RTSPAuthorize_Role			,    	"QTSS_RTSPAuthorize_Role", 		NULL },		//10
	{  QTSS_RTSPPreProcessor_Role		,    	"QTSS_RTSPPreProcessor_Role", 	NULL },	//11
	{  QTSS_RTSPRequest_Role			,    	"QTSS_RTSPRequest_Role", 		NULL },		//12
	{  QTSS_RTSPPostProcessor_Role		,    	"QTSS_RTSPPostProcessor_Role", 	NULL },	//13
	{  QTSS_RTSPSessionClosing_Role		,    	"QTSS_RTSPSessionClosing_Role", NULL },	//14
					
	{  QTSS_RTSPIncomingData_Role		,    	"QTSS_RTSPIncomingData_Role", 	NULL },	//15
					
		//RTP-specific
	{  QTSS_RTPSendPackets_Role			,    	"QTSS_RTPSendPackets_Role", 	NULL },	//16
	{  QTSS_ClientSessionClosing_Role	,    	"QTSS_ClientSessionClosing_Role", 	NULL },	//17
					
		//RTCP-specific
	{  QTSS_RTCPProcess_Role			,    	"QTSS_RTCPProcess_Role", 		NULL },	//17
					
		//File system roles
	{  QTSS_OpenFilePreProcess_Role		,    	"QTSS_OpenFilePreProcess_Role", NULL },	//18
	{  QTSS_OpenFile_Role				,    	"QTSS_OpenFile_Role", 			NULL },	//19
	{  QTSS_AdviseFile_Role				,    	"QTSS_AdviseFile_Role", 		NULL },	//20
	{  QTSS_ReadFile_Role				,    	"QTSS_ReadFile_Role", 			NULL },	//21
	{  QTSS_CloseFile_Role				,    	"QTSS_CloseFile_Role", 			NULL },	//22
	{  QTSS_RequestEventFile_Role		,    	"QTSS_RequestEventFile_Role", 	NULL }	//23
};

enum
{
	sRole_RoleName_Map_Size = 24,
};



/************************************************************************/
/************************************************************************/
//
//  RTSPSessionLog
//
/************************************************************************/
/************************************************************************/


	enum
	{
		kRTSPSes_ReadingRequest             = 0,
		kRTSPSes_FilteringRequest           = 1,
		kRTSPSes_RoutingRequest             = 2,
		kRTSPSes_AuthenticatingRequest      = 3,
		kRTSPSes_AuthorizingRequest         = 4,
		kRTSPSes_PreprocessingRequest       = 5,
		kRTSPSes_ProcessingRequest          = 6,
		kRTSPSes_SendingResponse            = 7,
		kRTSPSes_PostProcessingRequest      = 8,
		kRTSPSes_CleaningUp                 = 9,

	// 当RTSP协议通过HTTP隧道实现时将用到下面的状态 

		kRTSPSes_WaitingToBindHTTPTunnel = 10,                  // POST or GET side waiting to be joined with it's matching half
		kRTSPSes_SocketHasBeenBoundIntoHTTPTunnel = 11,         // POST side after attachment by GET side ( its dying )
		kRTSPSes_HTTPFilteringRequest = 12,                     // after kReadingRequest, enter this state
		kRTSPSes_ReadingFirstRequest = 13,                      // initial state - the only time we look for an HTTP tunnel
		kRTSPSes_HaveNonTunnelMessage = 14,                  // we've looked at the message, and its not an HTTP tunnle message
	};

	enum
	{
		sRTSP_State_StateName_Map_Size = 15,
	};


	ST_Enum_Str_Map RTSP_State_StateName_Map[] = 
	{
		{  kRTSPSes_ReadingRequest				,    	"kReadingRequest", 				NULL },		//0
		{  kRTSPSes_FilteringRequest			,    	"kFilteringRequest", 			NULL },		//1
		{  kRTSPSes_RoutingRequest				,    	"kRoutingRequest", 				NULL },		//2
		{  kRTSPSes_AuthenticatingRequest		,    	"kAuthenticatingRequest", 		NULL },		//3
		{  kRTSPSes_AuthorizingRequest			,    	"kAuthorizingRequest", 			NULL },		//4
		{  kRTSPSes_PreprocessingRequest		,    	"kPreprocessingRequest", 		NULL },		//5
		{  kRTSPSes_ProcessingRequest			,    	"kProcessingRequest", 			NULL },		//6
		{  kRTSPSes_SendingResponse				,    	"kSendingResponse", 			NULL },		//7
		{  kRTSPSes_PostProcessingRequest		,    	"kPostProcessingRequest", 		NULL },		//8
		{  kRTSPSes_CleaningUp					,    	"kCleaningUp", 					NULL },		//9
		{  kRTSPSes_WaitingToBindHTTPTunnel		,    	"kWaitingToBindHTTPTunnel", 	NULL },		//10
		{  kRTSPSes_SocketHasBeenBoundIntoHTTPTunnel,   "kSocketHasBeenBoundIntoHTTPTunnel", 			NULL },		//11
		{  kRTSPSes_HTTPFilteringRequest		,    	"kHTTPFilteringRequest", 		NULL },		//12
		{  kRTSPSes_ReadingFirstRequest			,    	"kReadingFirstRequest", 		NULL },		//13
		{  kRTSPSes_HaveNonTunnelMessage		,    	"kHaveNonTunnelMessage", 		NULL },		//14
	};


//
//
///************************************************************************/
///************************************************************************/
//// Description:	Get the Name of the Role with Switch Case
//// Parameters:
////	  inPrivateArgs: 
///************************************************************************/
///************************************************************************/
//
////Global
//static char* pQTSS_Register_Role			=    	"QTSS_Register_Role";
//static char* pQTSS_Initialize_Role			=    	"QTSS_Initialize_Role";
//static char* pQTSS_Shutdown_Role			=    	"QTSS_Shutdown_Role";
//
//static char* pQTSS_ErrorLog_Role			=    	"QTSS_ErrorLog_Role";
//static char* pQTSS_RereadPrefs_Role			=    	"QTSS_RereadPrefs_Role";
//static char* pQTSS_StateChange_Role			=    	"QTSS_StateChange_Role";
//
//static char* pQTSS_Interval_Role			=    	"QTSS_Interval_Role";
//
////RTSP-specific
//static char* pQTSS_RTSPFilter_Role			=    	"QTSS_RTSPFilter_Role";
//static char* pQTSS_RTSPRoute_Role			=    	"QTSS_RTSPRoute_Role";
//static char* pQTSS_RTSPAuthenticate_Role	=    	"QTSS_RTSPAuthenticate_Role";
//static char* pQTSS_RTSPAuthorize_Role		=    	"QTSS_RTSPAuthorize_Role";
//static char* pQTSS_RTSPPreProcessor_Role	=    	"QTSS_RTSPPreProcessor_Role";
//static char* pQTSS_RTSPRequest_Role			=    	"QTSS_RTSPRequest_Role";
//static char* pQTSS_RTSPPostProcessor_Role	=    	"QTSS_RTSPPostProcessor_Role";
//static char* pQTSS_RTSPSessionClosing_Role	=    	"QTSS_RTSPSessionClosing_Role";
//
//static char* pQTSS_RTSPIncomingData_Role	=    	"QTSS_RTSPIncomingData_Role";
//
////RTP-specific
//static char* pQTSS_RTPSendPackets_Role	    =    	"QTSS_RTPSendPackets_Role";
//static char* pQTSS_ClientSessionClosing_Role=    	"QTSS_ClientSessionClosing_Role";
//
////RTCP-specific
//static char* pQTSS_RTCPProcess_Role			=    	"QTSS_RTCPProcess_Role";
//
////File system roles
//static char* pQTSS_OpenFilePreProcess_Role	=    	"QTSS_OpenFilePreProcess_Role";
//static char* pQTSS_OpenFile_Role			=    	"QTSS_OpenFile_Role";
//static char* pQTSS_AdviseFile_Role			=    	"QTSS_AdviseFile_Role";
//static char* pQTSS_ReadFile_Role			=    	"QTSS_ReadFile_Role";
//static char* pQTSS_CloseFile_Role			=    	"QTSS_CloseFile_Role";
//static char* pQTSS_RequestEventFile_Role	=    	"QTSS_RequestEventFile_Role";
//
//
//#define GET_ROLE_NAME_CASE(QTSS_ROLE_NAME) \
//	case ROLE_NAME: \
//{ \
//	char* pROLE_NAME = "QTSS_ROLE_NAME"; \
//	memcpy(pRoleInfo, pROLE_NAME, sizeof(pROLE_NAME) ); \
//	break; \
//}
////
//
//
//int getRoleName2(QTSS_Role inRole, char* pRoleInfo)
//{
//	
//	switch (inRole)
//	{
//		//Global
//    case QTSS_Register_Role: 
//		memcpy(pRoleInfo, pQTSS_Register_Role, sizeof(pQTSS_Register_Role));
//		break;
//
//    case QTSS_Initialize_Role: 
//		memcpy(pRoleInfo, pQTSS_Initialize_Role, sizeof(pQTSS_Initialize_Role)); 
//		break;
//    case QTSS_Shutdown_Role: 
//		memcpy(pRoleInfo, pQTSS_Shutdown_Role, sizeof(pQTSS_Shutdown_Role)); 
//		break;
//    
//    case QTSS_ErrorLog_Role: 
//		memcpy(pRoleInfo, pQTSS_ErrorLog_Role, sizeof(pQTSS_ErrorLog_Role)); 
//		break;
//    case QTSS_RereadPrefs_Role: 
//		memcpy(pRoleInfo, pQTSS_RereadPrefs_Role, sizeof(pQTSS_RereadPrefs_Role)); 
//		break;
//    case QTSS_StateChange_Role: 
//		memcpy(pRoleInfo, pQTSS_StateChange_Role, sizeof(pQTSS_StateChange_Role)); 
//		break;
//    
//    case QTSS_Interval_Role: 
//		memcpy(pRoleInfo, pQTSS_Interval_Role, sizeof(pQTSS_Interval_Role)); 
//		break;
//    
//    //RTSP-specific
//    case QTSS_RTSPFilter_Role: 
//		memcpy(pRoleInfo, pQTSS_RTSPFilter_Role, sizeof(pQTSS_RTSPFilter_Role)); 
//		break;
//    case QTSS_RTSPRoute_Role: 
//		memcpy(pRoleInfo, pQTSS_RTSPRoute_Role, sizeof(pQTSS_RTSPRoute_Role)); 
//		break;
//    case QTSS_RTSPAuthenticate_Role: 
//		memcpy(pRoleInfo, pQTSS_RTSPAuthenticate_Role, sizeof(pQTSS_RTSPAuthenticate_Role)); 
//		break;
//    case QTSS_RTSPAuthorize_Role: 
//		memcpy(pRoleInfo, pQTSS_RTSPAuthorize_Role, sizeof(pQTSS_RTSPAuthorize_Role)); 
//		break;
//    case QTSS_RTSPPreProcessor_Role: 
//		memcpy(pRoleInfo, pQTSS_RTSPPreProcessor_Role, sizeof(pQTSS_RTSPPreProcessor_Role)); 
//		break;
//                                        //Modules may opt to "steal" the request and return a client response.
//    case QTSS_RTSPRequest_Role: 
//		memcpy(pRoleInfo, pQTSS_RTSPRequest_Role, sizeof(pQTSS_RTSPRequest_Role)); 
//		break;
//    case QTSS_RTSPPostProcessor_Role: 
//		memcpy(pRoleInfo, pQTSS_RTSPPostProcessor_Role, sizeof(pQTSS_RTSPPostProcessor_Role)); 
//		break;
//    case QTSS_RTSPSessionClosing_Role: 
//		memcpy(pRoleInfo, pQTSS_RTSPSessionClosing_Role, sizeof(pQTSS_RTSPSessionClosing_Role)); 
//		break;
//
//    case QTSS_RTSPIncomingData_Role: 
//		memcpy(pRoleInfo, pQTSS_RTSPIncomingData_Role, sizeof(pQTSS_RTSPIncomingData_Role)); 
//		break;
//
//    //RTP-specific
//    case QTSS_RTPSendPackets_Role: 
//		memcpy(pRoleInfo, pQTSS_RTPSendPackets_Role, sizeof(pQTSS_RTPSendPackets_Role)); 
//		break;
//
//    case QTSS_ClientSessionClosing_Role: 
//		memcpy(pRoleInfo, pQTSS_ClientSessionClosing_Role, sizeof(pQTSS_ClientSessionClosing_Role)); 
//		break;
//    
//    //RTCP-specific
//    case QTSS_RTCPProcess_Role: 
//		memcpy(pRoleInfo, pQTSS_RTCPProcess_Role, sizeof(pQTSS_RTCPProcess_Role)); 
//		break;
//
//    //File system Role: memcpy(pRoleInfo, p, sizeof(p)); break;s
//    case QTSS_OpenFilePreProcess_Role: 
//		memcpy(pRoleInfo, pQTSS_OpenFilePreProcess_Role, sizeof(pQTSS_OpenFilePreProcess_Role)); 
//		break;
//		
//    case QTSS_OpenFile_Role: 
//		memcpy(pRoleInfo, pQTSS_OpenFile_Role, sizeof(pQTSS_OpenFile_Role)); 
//		break;
//
//    case QTSS_AdviseFile_Role: 
//		memcpy(pRoleInfo, pQTSS_AdviseFile_Role, sizeof(pQTSS_AdviseFile_Role)); 
//		break;
//
//    case QTSS_ReadFile_Role: 
//		memcpy(pRoleInfo, pQTSS_ReadFile_Role, sizeof(pQTSS_ReadFile_Role)); 
//		break;
//
//    case QTSS_CloseFile_Role: 
//		memcpy(pRoleInfo, pQTSS_CloseFile_Role, sizeof(pQTSS_CloseFile_Role)); 
//		break;
//
//    case QTSS_RequestEventFile_Role: 
//		memcpy(pRoleInfo, pQTSS_RequestEventFile_Role, sizeof(pQTSS_RequestEventFile_Role)); 
//		break;
//		
//	}
//	
//	return strlen(pRoleInfo);
//}
	

#endif // !defined(AFX_EnumStringMapping_H__29B3858A_4BDE_46B6_BDEC_EFBAF1DAF42C__INCLUDED_)
