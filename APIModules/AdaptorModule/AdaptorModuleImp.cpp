//////////////////////////////////////////////////////////////////////
// AdaptorModuleImp.cpp: by wuzengde, Oct,1,2007
// Implement the communication with relay server and redirect the request to the Adaptor
//////////////////////////////////////////////////////////////////////

#include "StreamingServerFiles.h"
#include "AdaptorModuleImp.h"

//#include "OSArrayObjectDeleter.h"
//#include "QTSSModuleUtils.h"
//#include "QTSSMemoryDeleter.h"
#include "QTSSLogging.h" 

#include "RsRTSPUtility.h" 
#include "ModuleFunction.h"

//#include "SDPUtils.h"

AdaptorModuleImp*  AdaptorModuleImp::fModuleImp = NULL;
//RSOSMutex g_Mutex;
IMutex g_Mutex;

/************************************************************************/
// STATIC DATA
//
//
//Static variable for writing the SDP
//

static  StrPtrLen sDVRNotValidMessage("Movie SDP is not valid.");

const   SInt16    sNumSDPVectors = 22;
static const StrPtrLen     kCacheControlHeader("must-revalidate");

// Attributes
static QTSS_AttributeID sBadDVRFileErr				= qtssIllegalAttrID;
static QTSS_AttributeID sExpectedDigitFilenameErr	= qtssIllegalAttrID;
static QTSS_AttributeID sDVRFileSessionAttr			= qtssIllegalAttrID;
static QTSS_AttributeID sSeekToNonexistentTimeErr	= qtssIllegalAttrID;



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AdaptorModuleImp* AdaptorModuleImp::GetInstance()
{
	if (NULL == fModuleImp)
	{
		fModuleImp = new AdaptorModuleImp();
	}
	return fModuleImp;
}


AdaptorModuleImp::AdaptorModuleImp() : fModuleAdaptor(&g_Mutex)
{
	fModuleImp = NULL;
}

AdaptorModuleImp::~AdaptorModuleImp()
{

}

/************************************************************************/
// Description:	Register the roles supported by the module, added property to module
/************************************************************************/
QTSS_Error  AdaptorModuleImp::Register(QTSS_Register_Params* inParams)
{
		fModuleAdaptor.Register(inParams);

		//
		//Refer to ModuleFunction::Register(QTSS_Register_Params* inParams);

		///////////////////////     Global ROLES    ///////////////////////////////
			(void)QTSS_AddRole(QTSS_Initialize_Role);			//Implemented
			(void)QTSS_AddRole(QTSS_Shutdown_Role);				//Implemented

		///////////////////////     RTSP-specific  ///////
			(void)QTSS_AddRole(QTSS_RTSPPreProcessor_Role);

		///////////////////////     RTP-specific  ////////
			(void)QTSS_AddRole(QTSS_RTPSendPackets_Role);		//Implemented
			(void)QTSS_AddRole(QTSS_ClientSessionClosing_Role);	//Implemented

		///////////////////////     RTCP-specific  ///////
		///////////////////////     File system roles ////

		// Tell the server our name!
		static char* sModuleName = "AdaptorModule";
		::strcpy(inParams->outModuleName, sModuleName);

	return QTSS_NoErr;
}



QTSS_Error  AdaptorModuleImp::Initialize(QTSS_Initialize_Params* inParams)
{
	fModuleAdaptor.Initialize(inParams);
	
    QTSSModuleUtils::Initialize(inParams->inMessages, inParams->inServer, inParams->inErrorLogStream);

    // Report to the server that this module handles DESCRIBE, SETUP, PLAY, PAUSE, and TEARDOWN
    static QTSS_RTSPMethod sSupportedMethods[] = { qtssDescribeMethod, qtssSetupMethod, qtssTeardownMethod, qtssPlayMethod, qtssPauseMethod };
    QTSSModuleUtils::SetupSupportedMethods(inParams->inServer, sSupportedMethods, 5);
	
	return QTSS_NoErr;
}



QTSS_Error  AdaptorModuleImp::Shutdown()
{
	QTSS_Error theError = fModuleAdaptor.Shutdown();
	if (fModuleImp)
	{
		delete fModuleImp;
		fModuleImp = NULL;
	}

	return theError;
}



QTSS_Error  AdaptorModuleImp::RTSP_ProcessPreRTSPRequest(QTSS_StandardRTSP_Params* inParams)
{
	QTSS_Error theError = QTSS_NoErr;

    QTSS_RTSPMethod* theMethod = NULL;
    UInt32 theMethodLen = 0;
	theError = QTSS_GetValuePtr(inParams->inRTSPRequest, qtssRTSPReqMethod, 0, (void**)&theMethod, &theMethodLen);
	if (	(theError!=QTSS_NoErr)  ||  (theMethodLen != sizeof(QTSS_RTSPMethod))    )
	{
		Assert( (!"Get RTSP Method Failed!") );
        return QTSS_RequestFailed;	
	}

    switch (*theMethod)
    {
        case qtssDescribeMethod:
            theError = DoDescribe(inParams);
            break;
        case qtssSetupMethod:
            theError = DoSetup(inParams);
            break;
        case qtssPlayMethod:
            theError = DoPlay(inParams);
            break;
        case qtssTeardownMethod:
			theError = DoTearDown(inParams);
            break;
        case qtssPauseMethod:
			theError = DoPause(inParams);
            break;
        default:
            break;
    }

    if (theError != QTSS_NoErr)
	{
        (void)QTSS_Teardown(inParams->inClientSession);
	}

	return theError;
}



QTSS_Error  AdaptorModuleImp::RTP_SendPackets(QTSS_RTPSendPackets_Params* )	//inParams
{
	return QTSS_NoErr;
}


//
//Ask the ModuleAdaptor to Delete the Session Object 
QTSS_Error  AdaptorModuleImp::RTP_ClientSessionClosing(QTSS_ClientSessionClosing_Params* inParams)
{
	fModuleAdaptor.RTP_ClientSessionClosing(inParams);
	return QTSS_NoErr;
}



// ------------------------------------------------------------------------------------------------
//3. ����SDP�ļ�
//4. ���SDK�ĺϷ��ԣ���д��SDP
// ------------------------------------------------------------------------------------------------
QTSS_Error  AdaptorModuleImp::DoDescribe(QTSS_StandardRTSP_Params* inParams)
{

	if (!ModuleFunction::isRequestSelfDefined(inParams)){
		return QTSS_NoErr;			
	}

	//2. ����ChannelElem����---------------------------------------------------------------------------------
		fModuleAdaptor.DoDescribe(inParams);
		//---------------------------------------------------------------------------------------------------

	//3. ����SDP�ļ�

		ResizeableStringFormatter theFullSDPBuffer(NULL,0);
		ModuleFunction::genDoDescribeSDP_ForDoDescribe(&theFullSDPBuffer, inParams);


	//4.���SDK�ĺϷ��ԣ���д��SDP
			UInt32 totalSDPLength = 0;
			StrPtrLen fullSDPBuffSPL(theFullSDPBuffer.GetBufPtr(),theFullSDPBuffer.GetBytesWritten());
			
		// ------------ Check the headers
			SDPContainer rawSDPContainer;
			rawSDPContainer.SetSDPBuffer( &fullSDPBuffSPL );  
			if (!rawSDPContainer.IsSDPBufferValid())
			{    
				return QTSSModuleUtils::SendErrorResponseWithMessage(inParams->inRTSPRequest, qtssUnsupportedMediaType, &sDVRNotValidMessage);
			}

			SDPLineSorter sortedSDP(&rawSDPContainer, 1);
			StrPtrLen *theSessionHeadersPtr = sortedSDP.GetSessionHeaders();
			StrPtrLen *theMediaHeadersPtr = sortedSDP.GetMediaHeaders();

		// ----------- write out the sdp
			SInt16 vectorIndex = 1;
			iovec theSDPVec[sNumSDPVectors];//1 for the RTSP header, 6 for the sdp header, 1 for the sdp body
			::memset(&theSDPVec[0], 0, sizeof(theSDPVec));

			totalSDPLength += ModuleFunction::WriteSDPHeader(/*NULL, */theSDPVec, &vectorIndex, theSessionHeadersPtr);
			totalSDPLength += ModuleFunction::WriteSDPHeader(/*NULL, */theSDPVec, &vectorIndex, theMediaHeadersPtr);

			Assert(theSDPVec[2].iov_base != NULL);
			//ok, we have a filled out iovec. Let's send the response!
    
			// Append the RTSP Header info.
			(void)QTSS_AppendRTSPHeader(inParams->inRTSPRequest, qtssCacheControlHeader,
											kCacheControlHeader.Ptr, kCacheControlHeader.Len);
			QTSSModuleUtils::SendDescribeResponse(inParams->inRTSPRequest, inParams->inClientSession,
											&theSDPVec[0], vectorIndex, totalSDPLength);    
	
	return QTSS_NoErr;
}


//1. ���� RTPStream���������������
//2. ����RTSPӦ��
//3. ����ModuleAdaptor���
//
//ע: ������λ�����ƶ���DoPlay�У�����ÿ����һ��DoPlay,�ͽ���һ��ConnectionElem��ʵ��һ��RTSP���ض��RTP����, 2007.11.30
QTSS_Error  AdaptorModuleImp::DoSetup(QTSS_StandardRTSP_Params* inParams)
{

	if (!ModuleFunction::isRequestSelfDefined(inParams)){
		return QTSS_NoErr;			
	}
	
	//
	//1. �������ĺϷ��ԣ�2008.1.30�İ汾��ʼ���ټ������ĺϷ��ԡ�
	//	if (theDigitStr == NULL)   
	//		return QTSSModuleUtils::SendErrorResponse(inParams->inRTSPRequest, qtssClientBadRequest, sExpectedDigitFilenameErr);

	//
    //2. Create a new RTP stream
	QTSS_Error theErr;
    QTSS_RTPStreamObject newStream = NULL;
    theErr = QTSS_AddRTPStream(inParams->inClientSession, inParams->inRTSPRequest, &newStream, 0);    	//qtss_printf("CreackDelete	DoSetup(AddRTPStream) RTSPSession = %ld,  TheStream=%ld, ChannelIndex = %d\n", inParams->inRTSPRequest, newStream, theFile->fRealtimeDataRelay->fChannelIndex);
    if (theErr != QTSS_NoErr){
		Assert(!"QTSS_AddRTPStream��������ʧ��");
        return theErr;
	}
	
	//
    //3. ����newStream��������� find the payload for this track ID (if applicable)
	UInt32 theTrackID = 0;	
    StrPtrLen thePayload("X-SV3V-ES/90000");
    UInt32 thePayloadType = qtssUnknownPayloadType;
    Float32 bufferDelay = (Float32) 3.0; // FIXME need a constant defined for 3.0 value. It is used multiple places
    SInt32 theTimescale = 90000;

    // Set the payload type, payload name & timescale of this track
    theErr = QTSS_SetValue(newStream, qtssRTPStrBufferDelayInSecs, 0, &bufferDelay, sizeof(bufferDelay));
    Assert(theErr == QTSS_NoErr);
    theErr = QTSS_SetValue(newStream, qtssRTPStrPayloadName, 0, thePayload.Ptr, thePayload.Len);
    Assert(theErr == QTSS_NoErr);
    theErr = QTSS_SetValue(newStream, qtssRTPStrPayloadType, 0, &thePayloadType, sizeof(thePayloadType));
    Assert(theErr == QTSS_NoErr);
    theErr = QTSS_SetValue(newStream, qtssRTPStrTimescale, 0, &theTimescale, sizeof(theTimescale));
    Assert(theErr == QTSS_NoErr);
    theErr = QTSS_SetValue(newStream, qtssRTPStrTrackID, 0, &theTrackID, sizeof(theTrackID));
    Assert(theErr == QTSS_NoErr);
    
    // Set the number of quality levels. Allow up to 6
    static UInt32 sNumQualityLevels = 5;		//the origin is 5
    theErr = QTSS_SetValue(newStream, qtssRTPStrNumQualityLevels, 0, &sNumQualityLevels, sizeof(sNumQualityLevels));
    Assert(theErr == QTSS_NoErr);
    
    // Get the SSRC of this track
    UInt32* theTrackSSRC = NULL;
    UInt32 theTrackSSRCSize = 0;
    (void)QTSS_GetValuePtr(newStream, qtssRTPStrSSRC, 0, (void**)&theTrackSSRC, &theTrackSSRCSize);
    // The RTP stream should ALWAYS have an SSRC assuming QTSS_AddStream succeeded.
    Assert((theTrackSSRC != NULL) && (theTrackSSRCSize == sizeof(UInt32)));

	//
	//4. Send the response

    // Our array has now been updated to reflect the fields requested by the client. send the setup response
    (void)QTSS_AppendRTSPHeader(inParams->inRTSPRequest, qtssCacheControlHeader,
                                kCacheControlHeader.Ptr, kCacheControlHeader.Len);
    //send the setup response
    (void)QTSS_SendStandardRTSPResponse(inParams->inRTSPRequest, newStream, 0); 


	// WZD ADDED Code  ----------------------------------------------------------------------------------
	fModuleAdaptor.DoSetup(inParams, newStream);
	//---------------------------------------------------------------------------------------------------

	
	return QTSS_NoErr;
}



//1. ����ModuleAdaptor��DoPlay����
//3. �豸������ԣ�������QTSS_Play
//4. ���ͱ�׼Ӧ��

QTSS_Error  AdaptorModuleImp::DoPlay(QTSS_StandardRTSP_Params* inParams)  
{

	if (!ModuleFunction::isRequestSelfDefined(inParams)){
		return QTSS_NoErr;			
	}

	fModuleAdaptor.DoPlay(inParams);

	char* theFullPathStr = NULL;
	QTSS_Error theErr = QTSS_GetValueAsString(inParams->inRTSPRequest, qtssRTSPReqFullRequest, 0, &theFullPathStr);
	QTSSCharArrayDeleter theQTSSCharArrayDeleter(theFullPathStr);

    UInt32 theLen = 0;
    Float64* theStartTime = 0;
    theErr = QTSS_GetValuePtr(inParams->inRTSPRequest, qtssRTSPReqStartTime, 0, (void**)&theStartTime, &theLen);
	if ((theErr != QTSS_NoErr) || (theLen != sizeof(Float64)))	{
        return QTSSModuleUtils::SendErrorResponse(  inParams->inRTSPRequest, qtssClientBadRequest, sSeekToNonexistentTimeErr);
	}

    // Set the movie duration and size parameters
    Float64 movieDuration = 36000000;
    (void)QTSS_SetValue(inParams->inClientSession, qtssCliSesMovieDurationInSecs, 0, &movieDuration, sizeof(movieDuration));
    
    UInt64 movieSize = 36000000;
    (void)QTSS_SetValue(inParams->inClientSession, qtssCliSesMovieSizeInBytes, 0, &movieSize, sizeof(movieSize));

    //
    // For the purposes of the speed header, check to make sure all tracks are over a reliable transport
    Bool16 allTracksReliable = true;
    
    // Set the timestamp & sequence number parameters for each track.
    QTSS_RTPStreamObject* theRef = NULL;
    for (   UInt32 theStreamIndex = 0;
            QTSS_GetValuePtr(inParams->inClientSession, qtssCliSesStreamObjects, theStreamIndex, (void**)&theRef, &theLen) == QTSS_NoErr;
            theStreamIndex++)
    {
        UInt32* theTrackID = NULL;
        theErr = QTSS_GetValuePtr(*theRef, qtssRTPStrTrackID, 0, (void**)&theTrackID, &theLen);
        Assert(theErr == QTSS_NoErr);
        Assert(theTrackID != NULL);
        Assert(theLen == sizeof(UInt32));
        
        //SInt16 theSeqNum = (*theFile)->fFile.GetNextTrackSequenceNumber(*theTrackID);
        //SInt32 theTimestamp = (*theFile)->fFile.GetSeekTimestamp(*theTrackID);
        SInt16 theSeqNum = 0;
        SInt32 theTimestamp = 0;
        
        Assert(theRef != NULL);
        Assert(theLen == sizeof(QTSS_RTPStreamObject));
        
        theErr = QTSS_SetValue(*theRef, qtssRTPStrFirstSeqNumber, 0, &theSeqNum, sizeof(theSeqNum));
        Assert(theErr == QTSS_NoErr);
        theErr = QTSS_SetValue(*theRef, qtssRTPStrFirstTimestamp, 0, &theTimestamp, sizeof(theTimestamp));
        Assert(theErr == QTSS_NoErr);

        if (allTracksReliable)
        {
            QTSS_RTPTransportType theTransportType = qtssRTPTransportTypeUDP;
            theLen = sizeof(theTransportType);
            theErr = QTSS_GetValue(*theRef, qtssRTPStrTransportType, 0, &theTransportType, &theLen);
            Assert(theErr == QTSS_NoErr);
            
            if (theTransportType == qtssRTPTransportTypeUDP)
                allTracksReliable = false;
        }
    }
    
    //Tell the server to start playing this movie. We do want it to send RTCP SRs, but we DON'T want it to write the RTP header
    theErr = QTSS_Play(inParams->inClientSession, inParams->inRTSPRequest, qtssPlayFlagsSendRTCP);
    if (theErr != QTSS_NoErr)
        return theErr;

    // qtssRTPSesAdjustedPlayTimeInMsec is valid only after calling QTSS_Play
    // theAdjustedPlayTime is a way to delay the sending of data until a time after the 
    // the Play response should have been received.
    SInt64* theAdjustedPlayTime = 0;
    theErr = QTSS_GetValuePtr(inParams->inClientSession, qtssCliSesAdjustedPlayTimeInMsec, 0, (void**)&theAdjustedPlayTime, &theLen);
    Assert(theErr == QTSS_NoErr);
    Assert(theAdjustedPlayTime != NULL);
    Assert(theLen == sizeof(SInt64));

    (void)QTSS_SendStandardRTSPResponse(inParams->inRTSPRequest, inParams->inClientSession, qtssPlayRespWriteTrackInfo);
	
	return QTSS_NoErr;

}




//
// ��ʱ��֧�ָ÷���
//
QTSS_Error  AdaptorModuleImp::DoPause(QTSS_StandardRTSP_Params* inParams)  
{

	if (!ModuleFunction::isRequestSelfDefined(inParams)){
		return QTSS_NoErr;			
	}

	fModuleAdaptor.DoPause(inParams);

	(void)QTSS_Pause(inParams->inClientSession);
    (void)QTSS_SendStandardRTSPResponse(inParams->inRTSPRequest, inParams->inClientSession, 0);

	return QTSS_NoErr;
}


//
// ��ʱ��֧�ָ÷���
//
QTSS_Error  AdaptorModuleImp::DoTearDown(QTSS_StandardRTSP_Params* inParams)  
{
	
	if (!ModuleFunction::isRequestSelfDefined(inParams)){
		return QTSS_NoErr;			
	}
	
	char* theFullPathStr = NULL;
	QTSS_Error theError = QTSS_GetValueAsString(inParams->inRTSPRequest, qtssRTSPReqFullRequest, 0, &theFullPathStr);
	QTSSCharArrayDeleter theQTSSCharArrayDeleter(theFullPathStr);
	
	fModuleAdaptor.DoTearDown(inParams);

	(void)QTSS_Teardown(inParams->inClientSession);
    (void)QTSS_SendStandardRTSPResponse(inParams->inRTSPRequest, inParams->inClientSession, 0);

	return theError;
}
