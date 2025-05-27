// ConnectionElem.cpp: implementation of the ModuleAdaptor class.
//
//////////////////////////////////////////////////////////////////////
/*================================================================================
Contains:	this class process the connection between the server and the adaptor
		  该类为server和DVR adpator之间的Bridge，存放了部分Server的代码以及部分Adaptor的代码，

History:  Author	Date		Version		Description of Changes
		---------	----------	--------	-------------------------
		Wu ZengDe	2007/10/10	ver0.0.1	Initial
================================================================================*/

#include "ConnectionElem.h"
#include "RsRTPUtility.h"
#include "StreamPackager.h"
#include "SafeStdLib.h"
#include "Assert.h"
#include "RTPSessionInterface.h"
#include "IniReader2.h"

//OSMutex g_OSMutex;
static int sPrintWouldBlockPacket=1;
/*================================================================================
ConnectionElem  部分
================================================================================*/

ConnectionElem::ConnectionElem(QTSS_RTPStreamObject inRTPStreamingObject, QTSS_Object inRTPSessionObj, const LPIMutex inMutex) : 
fRTPStreamObject(inRTPStreamingObject), 
fRTPSessionObject(inRTPSessionObj),
fSequenceNumber(1),
fbMetaDataSented(FALSE),
m_Mutex(inMutex)
{
	CIniReader2 theReader("VideoRelay.ini");
	MAX_DELAY_TIME = theReader.ReadInteger("RunServer", "MAX_DELAY_TIME", 20);
	if(MAX_DELAY_TIME < 1) 
	{
		MAX_DELAY_TIME = 1;
	}

	srand( (unsigned)time( NULL ) );
	fnDelayTime	= rand()%MAX_DELAY_TIME;
}


ConnectionElem::~ConnectionElem()
{
}



//调用该函数必须先Lock m_Mutex
unsigned long ConnectionElem::ReceiveData(	HANDLE /* hSender */,	Tag /* inSenderTag */, 
										  unsigned char* inData, unsigned long inLength, unsigned long inDataType, 
										  unsigned long* /* ioCallbackParam  */)
{
	assert(inLength >= RS_MIN_PAYLOAD_SIZE);

	unsigned long theSendCounts = 0;	//the number of data packets sent in current connection

	BOOL isBeginningOfWriteBurst = TRUE;
	QTSS_WriteFlags theFlags = qtssWriteFlagsIsRTP;
	if (isBeginningOfWriteBurst) theFlags |= qtssWriteFlagsWriteBurstBegin;

	QTSS_PacketStruct   thePacketStruct;
	thePacketStruct.packetData = inData;
	thePacketStruct.packetTransmitTime = QTSS_Milliseconds() + fnDelayTime;
	// Set Sequence Number
	StreamPackager::setSeqNumber((unsigned char*)thePacketStruct.packetData, htons(fSequenceNumber));
	fSequenceNumber = (fSequenceNumber + 1) % USHRT_MAX;
			
	//WuZengDe_PrintPacketData(inData, inLength);		//对数据进行分析，把协议数据打印出来
	if(!fRTPStreamObject) 
	{
		qtss_printf("fRTPStreamObject = 0   ConnectionElem = %d\n", this);
		return 0;
	}

	if(fRTPStreamObject)
	{
		QTSS_Error theErr = QTSS_Write(fRTPStreamObject, &thePacketStruct , inLength, NULL, theFlags);

		int theWaitingTime = 0;
		while (inDataType == DATA_TYPES_NET_DVR_SYSHEAD && theErr == QTSS_WouldBlock && theWaitingTime < WouldBlock_SleepMaxTime)
		{
			theErr = QTSS_Write(fRTPStreamObject, &thePacketStruct, inLength, NULL, theFlags);
			Sleep(WouldBlock_SleepUnitTime);
			theWaitingTime+=WouldBlock_SleepUnitTime;
		}

		//RsRTPUtility::printRTPInfo((unsigned char*)thePacketStruct.packetData, inLength);

		//TBD, 返回三种状态, QTSS_WouldBlock	QTSS_BadArgument	QTSS_NotConnected，目前只对QTSS_NotConnected进行处理
		theSendCounts = 1;
		if (theErr != QTSS_NoErr)
		{
			QTSS_Write_Error_Proc(theErr, inLength);	
			theSendCounts = 0;
		}
		
		isBeginningOfWriteBurst = false;

		//if (ioCallbackParam){		//	*ioCallbackParam = theErr;		//}
		//qtss_printf("hSender = 0x%lX, inSenderTag = 0x%lX, inLength = %ld, fSequenceNumber = %lx, CallbackParam = %ld\n", hSender, (unsigned long)inSenderTag, inLength, fSequenceNumber, ioCallbackParam);
	}
	return theSendCounts; 

};



//QTSS_Write(...)返回值的处理
//		QTSS_WouldBlock:  	流的缓冲数据在这个时刻不能完全被写入
//		QTSS_BadArgument:   如果参数不正确
//		QTSS_NotConnected:  流的接受方已经不再处于连接状态
void ConnectionElem::QTSS_Write_Error_Proc(QTSS_Error theErr, unsigned long inLength)
{
	if (sPrintWouldBlockPacket)
	{
		if ( theErr == QTSS_WouldBlock ){
			qtss_printf("ConnectionElem::ProcessData(...)   WouldBlock    inLength = %ld\n", inLength);
		}else if ( theErr == QTSS_BadArgument){
			qtss_printf("ConnectionElem::ProcessData(...)   BadArgument    inLength = %ld\n", inLength);

		}else if ( theErr == QTSS_NotConnected){
			qtss_printf("ConnectionElem::ProcessData(...)   NotConnected    inLength = %ld\n", inLength);
		}else
		{
			qtss_printf("ConnectionElem::ProcessData(...)   theErr = %d    inLength = %ld\n", theErr, inLength);
		}

	}

}
