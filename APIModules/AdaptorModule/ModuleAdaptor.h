// ModuleAdaptor.h: interface for the ModuleAdaptor class.
//  
// The relation between the different modules
// Module --> ModuleImp
//                     --> ModuleAdaptor
//                     --> ModuleFunction
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ModuleAdaptor_H__FC9086CA_B115_4415_9F9F_38D3C1E0DAAE__INCLUDED_)
#define AFX_ModuleAdaptor_H__FC9086CA_B115_4415_9F9F_38D3C1E0DAAE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StreamingServerFiles.h"
#include "IDataReceiver.h"
#include "ChannelSharedMapping.h"
#include "boost/scoped_ptr.hpp"

class ModuleAdaptor
{
public:
	ModuleAdaptor(const LPIMutex in_Mutex);
	virtual ~ModuleAdaptor();

	///////////////////////     Global ROLES    ///////////////////////////////
	QTSS_Error  Register(QTSS_Register_Params* inParams);
	QTSS_Error  Initialize(QTSS_Initialize_Params* inParams);
	QTSS_Error  Shutdown();

	///////////////////////     RTSP-specific  ///////////////////////////////
	///////////////////////     RTP-specific  ///////////////////////////////
	QTSS_Error	RTP_ClientSessionClosing(QTSS_ClientSessionClosing_Params* inParams)  ;		//客户端断开连接时调用该函数

	///////////////////////     File system roles  ///////////////////////////////	
	///////////////////////     Specic RTSP Process  /////////////////////////////
	QTSS_Error DoDescribe(QTSS_StandardRTSP_Params* inParams);
	QTSS_Error DoSetup(QTSS_StandardRTSP_Params* inParams, QTSS_RTPStreamObject/*Void* */ inStream)  ;
	QTSS_Error DoPlay(QTSS_StandardRTSP_Params* inParams)  ;

	QTSS_Error DoPause(QTSS_StandardRTSP_Params* inParams)  ;				//该函数没有用到		
	QTSS_Error DoTearDown(QTSS_StandardRTSP_Params* inParams)  ;			//该函数没有用到

private:
	boost::scoped_ptr<ChannelSharedMapping> m_ChannelMapping;
	const LPIMutex m_Mutex;

	QTSS_Error Create_Connection_Channel(QTSS_ClientSessionObject inSessObj, LP_S_ConnectionElem inConnection, char* inRequest);
	QTSS_Error Delete_Connection_Channel(QTSS_ClientSessionObject inSessObj, LP_S_ConnectionElem inConnection, char* inRequest);
};

#endif // !defined(AFX_ModuleAdaptor_H__FC9086CA_B115_4415_9F9F_38D3C1E0DAAE__INCLUDED_)
