// ModuleFunction.cpp: implementation of the ModuleFunction class.
//
//////////////////////////////////////////////////////////////////////

#include "ModuleFunction.h"
#include "MyAssert.h"

#include "OSArrayObjectDeleter.h"
#include "QTSSModuleUtils.h"
#include "QTSSMemoryDeleter.h"
#include "QTSSLogging.h"
 
#include "SDPUtils.h"
#include "RsRTSPUtility.h"

/************************************************************************/
// STATIC DATA

//
//
//Static variable for writing the SDP
//
static  StrPtrLen sVersionHeader("v=0");
static  StrPtrLen sSessionNameHeader("s=");
static  StrPtrLen sPermanentTimeHeader("t=0 0");
static  StrPtrLen sConnectionHeader("c=IN IP4 0.0.0.0");
static  StrPtrLen sStaticControlHeader("a=control:*");
static  StrPtrLen sEmailHeader("e=Adim@");
static  StrPtrLen sURLHeader("u=http://");
static  StrPtrLen sEOL("\r\n");
static  StrPtrLen sDVRNotValidMessage("Movie SDP is not valid.");

// The media information
static  StrPtrLen sMediaRange("a=range:npt=0-  60.00");
static  StrPtrLen sMediaFormat("m=video 0 RTP/AVP 96");
static  StrPtrLen sMediaBandwidth("b=AS:79");
static  StrPtrLen sMediaRtpmap("a=rtpmap:96 X-SV3V-ES/90000");
static  StrPtrLen sMediaControl("a=control:trackID=1");

const   SInt16    sNumSDPVectors = 22;									//Not used
static const StrPtrLen     kCacheControlHeader("must-revalidate");		//Not used


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ModuleFunction::ModuleFunction()
{

}

ModuleFunction::~ModuleFunction()
{

}


//
//This function is only used for reference, which role for which use
QTSS_Error  ModuleFunction::Register(QTSS_Register_Params* /*inParams*/)
{
// 	Assert(!"This function is only used for reference, Please don't call me!");
// 	
// 	///////////////////////     Global ROLES    ///////////////////////////////
// 	//(void)QTSS_AddRole(QTSS_Register_Role);			//Default
// 	(void)QTSS_AddRole(QTSS_Initialize_Role);			//Implemented
// 	(void)QTSS_AddRole(QTSS_Shutdown_Role);				//Implemented
// 	
// 	//if (ALL_ROLE_ENABLED)
// 	{
// 		(void)QTSS_AddRole(QTSS_ErrorLog_Role);
// 		(void)QTSS_AddRole(QTSS_RereadPrefs_Role);
// 		(void)QTSS_AddRole(QTSS_StateChange_Role);
// 		(void)QTSS_AddRole(QTSS_Interval_Role);
// 		
// 		///////////////////////     RTSP-specific  ///////
// 		(void)QTSS_AddRole(QTSS_RTSPFilter_Role);
// 		(void)QTSS_AddRole(QTSS_RTSPRoute_Role);
// 		//(void)QTSS_AddRole(QTSS_RTSPAuthenticate_Role);
// 		//(void)QTSS_AddRole(QTSS_RTSPAuthorize_Role);
// 	}
// 	
// 	(void)QTSS_AddRole(QTSS_RTSPPreProcessor_Role);			//Implemented
// 	//if (ALL_ROLE_ENABLED)
// 	{
// 		(void)QTSS_AddRole(QTSS_RTSPRequest_Role);
// 		(void)QTSS_AddRole(QTSS_RTSPPostProcessor_Role);
// 		(void)QTSS_AddRole(QTSS_RTSPSessionClosing_Role);
// 		(void)QTSS_AddRole(QTSS_RTSPIncomingData_Role);
// 	}
// 	
// 	///////////////////////     RTP-specific  ////////
// 	(void)QTSS_AddRole(QTSS_RTPSendPackets_Role);		//Implemented
// 	(void)QTSS_AddRole(QTSS_ClientSessionClosing_Role);	//Implemented
// 	
// 	///////////////////////     RTCP-specific  ///////
// 	//if (ALL_ROLE_ENABLED)
// 	{
// 		(void)QTSS_AddRole(QTSS_RTCPProcess_Role);
// 		///////////////////////     File system roles ////
// 		(void)QTSS_AddRole(QTSS_OpenFilePreProcess_Role);
// 		(void)QTSS_AddRole(QTSS_OpenFile_Role);
// 		(void)QTSS_AddRole(QTSS_AdviseFile_Role);
// 		(void)QTSS_AddRole(QTSS_ReadFile_Role);
// 		(void)QTSS_AddRole(QTSS_CloseFile_Role);
// 		(void)QTSS_AddRole(QTSS_RequestEventFile_Role);
// 	}
// 	
	return QTSS_NoErr;
}


QTSS_Error ModuleFunction::genDoDescribeSDP_ForDoDescribe(ResizeableStringFormatter* inFullSDPBuffer, QTSS_StandardRTSP_Params* inParams)
{
	
	// Create SDP information
	
	//Get filename 
	char* fileNameStr = NULL;
	QTSS_Error theError = QTSS_GetValueAsString(inParams->inRTSPRequest, qtssRTSPReqFilePath, 0, &fileNameStr);
	QTSSCharArrayDeleter theQTSSCharArrayDeleter(fileNameStr);
	Assert(theError == QTSS_NoErr);
	//QTSSCharArrayDeleter fileNameStrDeleter(fileNameStr);  //2007.12.08¸ÄÎªtheQTSSCharArrayDeleter
    
	//Get IP addr
	StrPtrLen ipStr;
	(void)QTSS_GetValuePtr(inParams->inRTSPSession, qtssRTSPSesLocalAddrStr, 0, (void**)&ipStr.Ptr, &ipStr.Len);
	
	
	
	// *** The order of sdp headers is specified and required by rfc 2327
	
	// -------- version header 
	inFullSDPBuffer->Put(sVersionHeader);
	inFullSDPBuffer->Put(sEOL);
	
	// -------- owner header
	const SInt16 sLineSize = 256;
	char ownerLine[sLineSize]="";
	ownerLine[sLineSize - 1] = 0;
	char *ipCstr = ipStr.GetAsCString();
	OSCharArrayDeleter ipDeleter(ipCstr);
	// the first number is the NTP time used for the session identifier (this changes for each request)
	// the second number is the NTP date time of when the file was modified (this changes when the file changes)
	qtss_sprintf(ownerLine, "o=StreamingServer %"_64BITARG_"d %"_64BITARG_"d IN IP4 %s", (SInt64) OS::UnixTime_Secs() + 2208988800LU, (SInt64) OS::UnixTime_Secs() + 2208988800LU,ipCstr);
	Assert(ownerLine[sLineSize - 1] == 0);
	StrPtrLen ownerStr(ownerLine);
	inFullSDPBuffer->Put(ownerStr); 
	inFullSDPBuffer->Put(sEOL); 
	
	// -------- session header
	inFullSDPBuffer->Put(sSessionNameHeader);
	inFullSDPBuffer->Put(fileNameStr);
	inFullSDPBuffer->Put(sEOL);
	
	// -------- uri header
	inFullSDPBuffer->Put(sURLHeader);
	inFullSDPBuffer->Put(sEOL);
	
	// -------- email header
	inFullSDPBuffer->Put(sEmailHeader);
	inFullSDPBuffer->Put(sEOL);
	
	// -------- connection information header
	inFullSDPBuffer->Put(sConnectionHeader); 
	inFullSDPBuffer->Put(sEOL);
	
	// -------- time header
	// t=0 0 is a permanent always available movie (doesn't ever change unless we change the code)
	inFullSDPBuffer->Put(sPermanentTimeHeader);
	inFullSDPBuffer->Put(sEOL);
    
	// -------- control header
	inFullSDPBuffer->Put(sStaticControlHeader);
	inFullSDPBuffer->Put(sEOL);
	
	//////////////////////////////////////////////////////////////////////////
	// add the media information
	inFullSDPBuffer->Put(sMediaRange);
	inFullSDPBuffer->Put(sEOL);
	
	inFullSDPBuffer->Put(sMediaFormat);
	inFullSDPBuffer->Put(sEOL);
	
	inFullSDPBuffer->Put(sMediaBandwidth);
	inFullSDPBuffer->Put(sEOL);
	
	inFullSDPBuffer->Put(sMediaRtpmap);
	inFullSDPBuffer->Put(sEOL);
	
	inFullSDPBuffer->Put(sMediaControl);
	inFullSDPBuffer->Put(sEOL);
	
	//
	return QTSS_NoErr;
}


//
//
// Decide wheather the request is selfdefined
//
Bool16 ModuleFunction::isRequestSelfDefined(QTSS_StandardRTSP_Params* inParams)
{
	char* theFullPathStr = NULL;
	QTSS_Error theError = QTSS_GetValueAsString(inParams->inRTSPRequest, qtssRTSPReqFullRequest, 0, &theFullPathStr);
	QTSSCharArrayDeleter theQTSSCharArrayDeleter(theFullPathStr);
	Assert(theError == QTSS_NoErr);
	
	// Here we need to deal with the RTSP request according to our specification
	//StrPtrLen theFullPath(theFullPathStr);
	//
	Bool16 theSelfDefined = FALSE;
	if (RsRTSPUtility::isURLSelfDefined(theFullPathStr))
	{
		theSelfDefined = TRUE;
	}
	//
	return theSelfDefined;
}




/************************************************************************/
// Description:	Write the buffer to the stream
// Parameters: By wuzengde, OCT,1,2007
//	  hAdaptor: 
/************************************************************************/
UInt32 ModuleFunction::WriteSDPHeader(/*FILE* sdpFile, */iovec *theSDPVec, SInt16 *ioVectorIndex, StrPtrLen *sdpHeader)
{
	
    Assert (ioVectorIndex != NULL);
    Assert (theSDPVec != NULL);
    Assert (sdpHeader != NULL);
    Assert (*ioVectorIndex < sNumSDPVectors); // if adding an sdp param you need to increase sNumSDPVectors
    
    SInt16 theIndex = *ioVectorIndex;
    *ioVectorIndex += 1;
	
    theSDPVec[theIndex].iov_base =  sdpHeader->Ptr;
    theSDPVec[theIndex].iov_len = sdpHeader->Len;
    
    //if (sdpFile !=NULL)
    //    ::fwrite(theSDPVec[theIndex].iov_base,theSDPVec[theIndex].iov_len,sizeof(char),/*sdpFile*/NULL);
    
    return theSDPVec[theIndex].iov_len;
}
