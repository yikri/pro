// StreamPackager.cpp: implementation of the ModuleAdaptor class.
//
//////////////////////////////////////////////////////////////////////
/*================================================================================
Contains:	
================================================================================*/





#include "StreamPackager.h"
#include "QTSSMemoryDeleter.h"

//Set the RTP Version and Data type
StreamPackager::StreamPackager() 
{
	//
	// RTPPacketBuffer
	memset(fRTPPacketBuffer, 0, RS_RTP_TOTAL_SIZE* sizeof(char));
	unsigned int nRTPVersion = htons(0x80E0);
	memcpy((char *)fRTPPacketBuffer, (char *)&nRTPVersion, 2 * sizeof(unsigned char));
	//
	DATA_TYPES_DEF theDataType;
	theDataType = DATA_TYPES_NET_DVR_STREAMDATA;
	memcpy(fRTPPacketBuffer + RS_RTP_HEAD_DATA_TYPE_BYTE, (unsigned char*)&theDataType, sizeof(DATA_TYPES_DEF) );

	//
	//Stream MetaDataBuffer
	memset(fRTPMetaDataBuffer, 0, RS_RTP_TOTAL_SIZE* sizeof(char));
	memcpy((char *)fRTPMetaDataBuffer, (char *)&nRTPVersion, 2 * sizeof(unsigned char));
	//
	theDataType = DATA_TYPES_NET_DVR_SYSHEAD;
	memcpy(fRTPMetaDataBuffer + RS_RTP_HEAD_DATA_TYPE_BYTE, (unsigned char*)&theDataType, sizeof(DATA_TYPES_DEF));
	//
	fRTPMetaDataBuffer_Len = 0;
	fbMetaDataSupported = TRUE;			//初始必须设置为TRUE
	//
};




unsigned char* StreamPackager::getDataPacket(Tag inTag, unsigned char* inBuffer, unsigned long inBufferLen, unsigned long* outPacketSize)
{
	
	Assert(inBufferLen +  RS_RTP_TAG_HEAD_SIZE <= RS_MAX_PACKET_SIZE );
	*outPacketSize = RS_RTP_TAG_HEAD_SIZE + inBufferLen;
	
	memcpy(fRTPPacketBuffer + RS_RTP_HEAD_SIZE, (unsigned char*) &inTag, sizeof(inTag));
	memcpy(fRTPPacketBuffer + RS_RTP_TAG_HEAD_SIZE, inBuffer, inBufferLen * sizeof(unsigned char));
	return fRTPPacketBuffer;
}


//第一次调用函数函数，把相关Buffer存入Buffer
unsigned char* StreamPackager::getSet_MetaDataPacket(Tag inTag, unsigned char* inBuffer, unsigned long inBufferLen, unsigned long* outPacketSize)
{
	
	if ( 0 == inBufferLen ) fbMetaDataSupported = FALSE;
	
	Assert(inBufferLen +  RS_RTP_TAG_HEAD_SIZE <= RS_MAX_PAYLOAD_SIZE );
	fRTPMetaDataBuffer_Len  = RS_RTP_TAG_HEAD_SIZE + inBufferLen;
	*outPacketSize = fRTPMetaDataBuffer_Len;
	
	memcpy(fRTPMetaDataBuffer + RS_RTP_HEAD_SIZE, (unsigned char*) &inTag, sizeof(inTag));
	memcpy(fRTPMetaDataBuffer + RS_RTP_TAG_HEAD_SIZE, inBuffer, inBufferLen * sizeof(unsigned char));

	return fRTPMetaDataBuffer;
} 
 
//第二次可直接从Buffer取
unsigned char* StreamPackager::getMetaDataPacket(unsigned long* outPacketSize)
{
	unsigned char* theDataPacket = NULL;
	*outPacketSize = fRTPMetaDataBuffer_Len;
	if (isStreamMetaDataAvailable())
	{
		theDataPacket = fRTPMetaDataBuffer;
	}else
	{
		;//DoNothing
	}
	return theDataPacket;
}
