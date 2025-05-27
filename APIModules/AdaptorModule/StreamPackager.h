/*================================================================================
StreamPackage.h: 该类封装了打包细节
#include "StreamPackage.h"
================================================================================*/

#if !defined(AFX_ModuleAdaptor_H__STREAM_PACKAGE__INCLUDED_)
#define AFX_ModuleAdaptor_H__STREAM_PACKAGE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "lkRTPPacketFormat.h"
#include "StreamingServerFiles.h"
#include "LeKuo.h"


/*

	0                   1                   2                   3
	0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|V=2|P|X|  CC   |M|     PT      |       sequence number         |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|                           timestamp                           |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|           synchronization source (SSRC) identifier            |
	+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
	|            contributing source (CSRC) identifiers             |
	|                             ....                              |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/



//
//封装组包过程类

class StreamPackager
{
public:
	//
	//1. setup the object
	StreamPackager();

	//
	//2. Stream MetaData related functions

	BOOL isMetaDataSupported() {return fbMetaDataSupported;};

	//
	//2.1 第一次调用，传入Buffer参数，把数据存入fRTPMetaDataBuffer特定区域，并且返回MetaData结构
	unsigned char* getSet_MetaDataPacket(Tag inTag, unsigned char* inBuffer, unsigned long inBufferLen, unsigned long* outPacketSize);
	
	//
	//2.2 第二次可直接获取
	unsigned char* getMetaDataPacket(unsigned long* outPacketSize);

	//
	//3. getStreamPacket
	unsigned char* getDataPacket(Tag inTag, unsigned char* inBuffer, unsigned long inBufferLen, unsigned long* outPacketSize);

	static unsigned char* setSeqNumber(unsigned char* inBuffer, UInt16 inNum) 
	{
		memcpy(inBuffer + 2, &inNum, sizeof(UInt16));
		return inBuffer;
	};

private:
	//
	BOOL isStreamMetaDataAvailable() {return fRTPMetaDataBuffer_Len > 0; };
	//
	unsigned char fRTPPacketBuffer[RS_RTP_TOTAL_SIZE];		//RTP大小， RTP_TOTAL_SIZE = (RS_RTP_HEAD_SIZE_12 + RS_RTP_TAG_SIZE_8 + RS_MAX_PAYLOAD_SIZE_1024);
	unsigned char fRTPMetaDataBuffer[RS_RTP_TOTAL_SIZE];	//MetaData，对某个Channel来说MetaData是相同的
	unsigned long fRTPMetaDataBuffer_Len ;

	//开始设置为TRUE, 根据getMetaDataPacket函数传入的inBufferLen确定
	BOOL fbMetaDataSupported;		// If the inBufferLen is Zero, then the MetaData Is not Supported
};


#endif // !defined(AFX_ModuleAdaptor_H__STREAM_PACKAGE__INCLUDED_)
