/*================================================================================
StreamPackage.h: �����װ�˴��ϸ��
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
//��װ���������

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
	//2.1 ��һ�ε��ã�����Buffer�����������ݴ���fRTPMetaDataBuffer�ض����򣬲��ҷ���MetaData�ṹ
	unsigned char* getSet_MetaDataPacket(Tag inTag, unsigned char* inBuffer, unsigned long inBufferLen, unsigned long* outPacketSize);
	
	//
	//2.2 �ڶ��ο�ֱ�ӻ�ȡ
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
	unsigned char fRTPPacketBuffer[RS_RTP_TOTAL_SIZE];		//RTP��С�� RTP_TOTAL_SIZE = (RS_RTP_HEAD_SIZE_12 + RS_RTP_TAG_SIZE_8 + RS_MAX_PAYLOAD_SIZE_1024);
	unsigned char fRTPMetaDataBuffer[RS_RTP_TOTAL_SIZE];	//MetaData����ĳ��Channel��˵MetaData����ͬ��
	unsigned long fRTPMetaDataBuffer_Len ;

	//��ʼ����ΪTRUE, ����getMetaDataPacket���������inBufferLenȷ��
	BOOL fbMetaDataSupported;		// If the inBufferLen is Zero, then the MetaData Is not Supported
};


#endif // !defined(AFX_ModuleAdaptor_H__STREAM_PACKAGE__INCLUDED_)
