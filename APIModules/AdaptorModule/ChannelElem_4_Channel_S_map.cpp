#include "ChannelElem_4_Channel_S_map.h"
#include "rsPrintf.h"


ChannelElem_4_Channel_S_map::ChannelElem_4_Channel_S_map(const LPIDVR inDVR, const char* inCreateStr, Tag inTag, const LPIMutex inMutex): 
m_pDVR(inDVR), m_ChannelCreateStr(inCreateStr),
m_Tag(inTag), m_Mutex(inMutex)
{
	assert(m_pDVR);
	//assert(m_pDeviceManager);

	//m_Channel.reset(m_pDVR->CreateChannel(m_ChannelCreateStr.c_str(), m_Tag, m_Mutex));	//�Ƿ񴴽���ChannelSharedMapping������ 
	m_Channel = m_pDVR->CreateChannel(m_ChannelCreateStr.c_str(), m_Tag, m_Mutex);
	//assert(m_Channel.get());
}


ChannelElem_4_Channel_S_map::~ChannelElem_4_Channel_S_map(void)
{
	//if(m_Channel.get())
	{
		m_Channel->Stop();
		m_Channel->Detach_DataReceiver();
		m_pDVR->DestroyChannel(m_Channel);
		m_Channel = NULL;
	}
}


LK_Error ChannelElem_4_Channel_S_map::Attach_DataReceiver(LP_S_ConnectionElem inConnection)
{
	m_ConnectionList.remove(inConnection);
	m_ConnectionList.push_back(inConnection);
	return LK_NoErr;
}


LK_Error ChannelElem_4_Channel_S_map::Detach_DataReceiver(LP_S_ConnectionElem inConnection)
{
	m_ConnectionList.remove(inConnection);
	return LK_NoErr;
}


LK_Error ChannelElem_4_Channel_S_map::Play(const char* inPlayOption)
{
	//assert(m_Channel.get());	
	m_Channel->Attach_DataReceiver(this);

	m_Channel->Play(inPlayOption);
	return LK_NoErr;
}


BOOL ChannelElem_4_Channel_S_map::IsPlaying()
{
	return m_Channel->IsPlaying();
}


LK_Error ChannelElem_4_Channel_S_map::Stop()
{
	return m_Channel->Stop();
}


MetadataLen ChannelElem_4_Channel_S_map::getMetadataLen()
{
	return m_Channel->getMetadataLen();
}


MetadataLen ChannelElem_4_Channel_S_map::getMetadata(unsigned char* outDataBuffer, INT32 inBufferLength)
{
	return m_Channel->getMetadata(outDataBuffer, inBufferLength);
}


void ChannelElem_4_Channel_S_map::ProcessData_PrintPara(HANDLE hSender, Tag inSenderTag, 
														unsigned char* /* inData */, unsigned long inLength, unsigned long /* inDataType */, 
														unsigned long* ioCallbackParam)
{
	rsPrintf("hSender = 0x%lX, inSenderTag = 0x%lX, inLength = %ld, CallbackParam = %ld\n", 
		hSender, (unsigned long)inSenderTag, inLength, ioCallbackParam);
}

#define DIVIDE_DATA_INTO_SMALL_PACKET 0


//�ú�����DVR�̵߳��ã�����ú�����������Mutex
unsigned long ChannelElem_4_Channel_S_map::ReceiveData(	HANDLE hSender, Tag inSenderTag, 
													   unsigned char* inData, unsigned long inLength, unsigned long inDataType, 
													   unsigned long* ioCallbackParam )
{
	//OSMutexLocker theLocker(&g_OSMutex);
	ProcessData_PrintPara(hSender, inSenderTag, inData, inLength, inDataType, ioCallbackParam);

	unsigned long theSendCounts = 0;
	//int theClientID_ForDebug = 0;

	for (Connection_S_List_iter pos = m_ConnectionList.begin(); pos != m_ConnectionList.end(); ++pos){
		//theClientID_ForDebug ++;
		//qtss_printf("ChannelElem::ProcessData �������ݵ���    %d      ���ͻ���\n", theClientID_ForDebug);

		LP_S_ConnectionElem theConnectElem = *pos;

		//1. ���ȴ���MetaData
		theSendCounts = SendMetaData(theConnectElem, inDataType, inData, inLength, hSender, inSenderTag, ioCallbackParam);

		//
		//2. ��������

#if DIVIDE_DATA_INTO_SMALL_PACKET

		//2.1 calc how may copys needed to be divided
		int theSendTimes = inLength / RS_MAX_PAYLOAD_SIZE;
		unsigned long theLastDataLen = inLength % RS_MAX_PAYLOAD_SIZE;
		if( 0 != theLastDataLen) 	theSendTimes++;

		for (int i = 0; i < theSendTimes; i++){

			unsigned char* theDataLoc = inData + i*RS_MAX_PAYLOAD_SIZE;
			unsigned long theDataLen = RS_MAX_PAYLOAD_SIZE;

			//
			// ע: ������һ��������:  Ϊ�˱������ݰ����͹�С�����������������һ������С���ض��ĳ��ȣ� �����һ�����ݰ��ϲ���ǰ��һ�����ݰ���
			//   (theSendTimes -1)  == i ��ʾ�������һ�����ݰ�
			//   (theSendTimes -2)  == i ��ʾ���ǵ����ڶ������ݰ�
			if (	((theSendTimes -1)  == i)		&&		(theLastDataLen >0)		){//�������һ�����ݴ�С

				theDataLen = theLastDataLen;

			}else if ( (theSendTimes -2)  == i){

				if (theLastDataLen <= RS_MIN_PAYLOAD_SIZE)
				{
					theDataLen = theLastDataLen + RS_MAX_PAYLOAD_SIZE;
					i++; //��ʾ�������������ݰ�
				};
			}

			//
			//set the data into packet

			unsigned long theBufferLen_InPacket = 0;
			unsigned char* theBuffer_InPacket = m_Packager.getDataPacket(m_Tag, theDataLoc, theDataLen, &theBufferLen_InPacket);			
			theSendCounts += theConnectElem->ReceiveData(	hSender, inSenderTag, 	theBuffer_InPacket, theBufferLen_InPacket, DATA_TYPES_NET_DVR_STREAMDATA, ioCallbackParam);
		}

#else
		//! DIVIDE_DATA_INTO_SMALL_PACKET

		unsigned long theBufferLen_InPacket = 0;
		unsigned char* theBuffer_InPacket = m_Packager.getDataPacket(m_Tag, inData, inLength, &theBufferLen_InPacket);			
		theSendCounts += theConnectElem->ReceiveData(	hSender, inSenderTag, 
			theBuffer_InPacket, theBufferLen_InPacket, DATA_TYPES_NET_DVR_STREAMDATA, 
			ioCallbackParam);

#endif


	}//for all connections 

	return theSendCounts;
}


unsigned long ChannelElem_4_Channel_S_map::SendMetaData( LP_S_ConnectionElem theConnectElem, unsigned long inDataType, unsigned char* inData, unsigned long inLength, HANDLE hSender, Tag inSenderTag, unsigned long* ioCallbackParam )
{
	unsigned long theSendCounts = 0;

	if (m_Packager.isMetaDataSupported())	{

		if(!theConnectElem->isMetaDataSented())	{

			theConnectElem->setMetaDataSented(TRUE);

			//a. Get the MetaData
			unsigned long theMetaLen_InPacket = 0;
			unsigned char* theMetaBuffer_InPacket = m_Packager.getMetaDataPacket(&theMetaLen_InPacket);
			if (!theMetaBuffer_InPacket){

				//i.  ���Pure MetaData
				if(DATA_TYPES_NET_DVR_SYSHEAD == inDataType )
				{
					theMetaBuffer_InPacket = m_Packager.getSet_MetaDataPacket(m_Tag, inData, inLength, &theMetaLen_InPacket);
				}else
				{
					LPIChannel thehChannel = (LPIChannel) hSender;
					INT32 theMetaLen_ForGet = thehChannel->getMetadataLen();

					unsigned char theMetaBuffer_ForGet[RS_MAX_PAYLOAD_SIZE];
					thehChannel->getMetadata(theMetaBuffer_ForGet, RS_MAX_PAYLOAD_SIZE);

					//ii. ��Pure MetaDataͷ�����ϰ�ͷ
					theMetaBuffer_InPacket = m_Packager.getSet_MetaDataPacket(m_Tag, theMetaBuffer_ForGet, theMetaLen_ForGet, &theMetaLen_InPacket);
				}

			};

			//b. Send the Data
			if (m_Packager.isMetaDataSupported())
			{
				//for(int i = 0; i<10; i++)
				//{
				theSendCounts += theConnectElem->ReceiveData(	hSender,inSenderTag, 
					theMetaBuffer_InPacket, theMetaLen_InPacket, DATA_TYPES_NET_DVR_SYSHEAD,
					ioCallbackParam);
				//}//for
			}//if

		}

	}//����MetaData��if (m_Packager.isMetaDataSupported())
	return theSendCounts;
}
