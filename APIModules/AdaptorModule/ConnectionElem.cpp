// ConnectionElem.cpp: implementation of the ModuleAdaptor class.
//
//////////////////////////////////////////////////////////////////////
/*================================================================================
Contains:	this class process the connection between the server and the adaptor
		  ����Ϊserver��DVR adpator֮���Bridge������˲���Server�Ĵ����Լ�����Adaptor�Ĵ��룬

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
ConnectionElem  ����
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



//���øú���������Lock m_Mutex
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
			
	//WuZengDe_PrintPacketData(inData, inLength);		//�����ݽ��з�������Э�����ݴ�ӡ����
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

		//TBD, ��������״̬, QTSS_WouldBlock	QTSS_BadArgument	QTSS_NotConnected��Ŀǰֻ��QTSS_NotConnected���д���
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



//QTSS_Write(...)����ֵ�Ĵ���
//		QTSS_WouldBlock:  	���Ļ������������ʱ�̲�����ȫ��д��
//		QTSS_BadArgument:   �����������ȷ
//		QTSS_NotConnected:  ���Ľ��ܷ��Ѿ����ٴ�������״̬
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
