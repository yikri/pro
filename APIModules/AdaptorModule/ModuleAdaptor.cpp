/*================================================================================
History:  Author	Date		Version		Description of Changes
		---------	----------	--------	-------------------------
		Wu ZengDe	2007/10/10	ver0.0.1	Initial

通过m_ChannelMapping增加Channel对象的引用，返回theSessionInterface->m_LP_S_ChannelElem，并增加Connection对象的引用
================================================================================*/

//#include "QTSSMemoryDeleter.h"
//#include "RsRTSPUtility.h"
//#include "RsRTPUtility.h"
#include "ChannelSharedMapping.h"
#include "ModuleAdaptor.h"
#include "Log.h"
#include "RTPSessionInterface.h"
#include "QTSSMemoryDeleter.h"
#define _MODADPTOR_PRINTF_ 1


static QTSS_AttributeID sModAdaptor_LP_S_IChannel_Attr		= qtssIllegalAttrID;


ModuleAdaptor::ModuleAdaptor(const LPIMutex in_Mutex) : m_Mutex(in_Mutex)
{
	assert(in_Mutex);
	IMutexLocker theLocker(m_Mutex);

	m_ChannelMapping.reset(new ChannelSharedMapping(m_Mutex));
}


ModuleAdaptor::~ModuleAdaptor()
{}


//Add one ChannelElem and ConnectElem for every Connect Session
QTSS_Error  ModuleAdaptor::Register(QTSS_Register_Params* /* inParams */)
{
	static char*            sModAdaptor_LP_S_IChannel_Attr_Name	= "LP_S_IChannel";
	(void)QTSS_AddStaticAttribute(qtssClientSessionObjectType, sModAdaptor_LP_S_IChannel_Attr_Name, NULL, qtssAttrDataTypeVoidPointer);
    (void)QTSS_IDForAttr(qtssClientSessionObjectType, sModAdaptor_LP_S_IChannel_Attr_Name, &sModAdaptor_LP_S_IChannel_Attr);

	return QTSS_NoErr;
}


//	Call this when Initialize
QTSS_Error  ModuleAdaptor::Initialize(QTSS_Initialize_Params* /* inParams */)
{
	return QTSS_NoErr;
};
 
//	Call this when server shutdown
QTSS_Error  ModuleAdaptor::Shutdown()
{
	return QTSS_NoErr;
};


QTSS_Error ModuleAdaptor::DoDescribe(QTSS_StandardRTSP_Params* /* inParams */)  
{ 
	CLog::Log("=============================================ModuleAdaptor::DoDescribe(...)		Called");
	return QTSS_NoErr;	
};

//CreateConnection
QTSS_Error ModuleAdaptor::DoSetup(QTSS_StandardRTSP_Params* inParams, QTSS_RTPStreamObject inpStream) 
{ 
	CLog::Log("=============================================ModuleAdaptor::DoSetup(...)		Called");

	//1. Look up in attribute first, if not found, then create the Channel object

	char* theFullPathStr = NULL;
	QTSSCharArrayDeleter theDeleter(theFullPathStr);
	QTSS_Error theError = QTSS_GetValueAsString(inParams->inRTSPRequest, qtssRTSPReqFullRequest, 0, &theFullPathStr);
	theError;

	RTPSessionInterface* theSessionInterface = (RTPSessionInterface*) inParams->inClientSession;
	if(!theSessionInterface->m_LP_S_ConnectionElem.get())
	{
		IMutexLocker theLocker(m_Mutex);

		LP_S_ConnectionElem theElem = LP_S_ConnectionElem(new ConnectionElem(inpStream, inParams->inClientSession, m_Mutex));
		theSessionInterface->m_LP_S_ConnectionElem = theElem; 
	}

	return QTSS_NoErr; 
};

//	Create Channel, attach the Connection the Connection and Play Channel
QTSS_Error ModuleAdaptor::DoPlay(QTSS_StandardRTSP_Params* inParams)
{ 
	CLog::Log("=============================================ModuleAdaptor::DoPlay(...)		Called");

	char* theFullPathStr = NULL;
	QTSSCharArrayDeleter theDeleter(theFullPathStr);
	QTSS_Error theError = QTSS_GetValueAsString(inParams->inRTSPRequest, qtssRTSPReqFullRequest, 0, &theFullPathStr);
	theError;

	//2. Play Channel
	IMutexLocker theLocker(m_Mutex);
	RTPSessionInterface* theSessionInterface = (RTPSessionInterface*) inParams->inClientSession; 

	if(!theSessionInterface->m_LP_S_ChannelElem.get())
	{
		theSessionInterface->m_LP_S_ChannelElem = m_ChannelMapping->CreateChannel(theFullPathStr);
	}

	assert(theSessionInterface->m_LP_S_ConnectionElem.get());
	theSessionInterface->m_LP_S_ChannelElem->Attach_DataReceiver(theSessionInterface->m_LP_S_ConnectionElem);

	theSessionInterface->m_LP_S_ChannelElem->Play(theFullPathStr);

	return QTSS_NoErr; 
}; 



//
//Obsolete，replaced by function ModuleAdaptor::RTP_ClientSessionClosing(...)
QTSS_Error ModuleAdaptor::DoPause(QTSS_StandardRTSP_Params* /* inParams */)  
{ 
	CLog::Log("=============================================ModuleAdaptor::DoPause(...)		Called, Function Not Used");
	return QTSS_NoErr; 
};



//
// replaced by ModuleAdaptor::RTP_ClientSessionClosing(...)
QTSS_Error ModuleAdaptor::DoTearDown(QTSS_StandardRTSP_Params* inParams)  
{ 
	CLog::Log("=============================================ModuleAdaptor::DoTearDown(...)		Called, Function Not Used");

	IMutexLocker theLocker(m_Mutex);
	RTPSessionInterface* theSessionInterface = (RTPSessionInterface*) inParams->inClientSession;
	if(theSessionInterface->m_LP_S_ConnectionElem.get())
	{
		IMutexLocker theLocker(m_Mutex);
		theSessionInterface->m_LP_S_ChannelElem->Detach_DataReceiver(theSessionInterface->m_LP_S_ConnectionElem);
		theSessionInterface->m_LP_S_ConnectionElem->clearRTPStreamObject();
	}

	return QTSS_NoErr; 
};


//
//  Delete the channel object
QTSS_Error	ModuleAdaptor::RTP_ClientSessionClosing(QTSS_ClientSessionClosing_Params* inParams)
{
	CLog::Log("=============================================ModuleAdaptor::RTP_ClientSessionClosing(...)		Called");

	char* theFullPathStr = NULL;
	QTSSCharArrayDeleter theDeleter(theFullPathStr);
	QTSS_Error theError = QTSS_GetValueAsString(inParams->inClientSession, qtssRTSPReqFullRequest, 0, &theFullPathStr);
	theError;

	RTPSessionInterface* theSessionInterface = (RTPSessionInterface*) inParams->inClientSession;
	if(theSessionInterface->m_LP_S_ConnectionElem.get())
	{
		IMutexLocker theLocker(m_Mutex);
		Delete_Connection_Channel(inParams->inClientSession, theSessionInterface->m_LP_S_ConnectionElem, theFullPathStr);
	}

	return QTSS_NoErr;
}

//
//QTSS_Error ModuleAdaptor::Create_Connection_Channel(QTSS_ClientSessionObject inSessObj, LP_S_ConnectionElem inConnection, char* inRequest)
//{
//	IMutexLocker theLocker(m_Mutex);
//
//
//	return QTSS_NoErr;
//}

QTSS_Error ModuleAdaptor::Delete_Connection_Channel(QTSS_ClientSessionObject inSessObj, LP_S_ConnectionElem inConnection, char* /* inRequest */)
{
	IMutexLocker theLocker(m_Mutex);

	RTPSessionInterface* theSessionInterface = (RTPSessionInterface*) inSessObj;
	if(theSessionInterface->m_LP_S_ChannelElem.get())
	{
		theSessionInterface->m_LP_S_ChannelElem->Detach_DataReceiver(inConnection);
		theSessionInterface->m_LP_S_ChannelElem.reset();
	}
	else{
		assert(0);		//到此Channel应该不为空
	}

	assert(theSessionInterface->m_LP_S_ConnectionElem.get());
	theSessionInterface->m_LP_S_ConnectionElem.reset();

	return QTSS_NoErr;
}