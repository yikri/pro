// #include "ConnectionElem.h"

#pragma once

#include "StreamingServerFiles.h"
#include "IDataReceiver.h"
#include "RsBoostType.h"
#include "StdRsType.h"
//#include "RSOSMutex.h"

class ConnectionElem : public IDataReceiver
{
public:
	ConnectionElem(QTSS_RTPStreamObject inRTPStreamingObject, QTSS_Object inRTPSessionObj, const LPIMutex inMutex);
	virtual ~ConnectionElem();

	BOOL isMetaDataSented()					{return fbMetaDataSented;		};
	void setMetaDataSented(BOOL inbSented )	{fbMetaDataSented = inbSented;	};
	//ChannelElem* getChannelElem()			{return (ChannelElem*)getParentElem()->getQueueElem()->GetEnclosingObject();	};

	//to stop relay streaming, when session closes
	void clearRTPStreamObject()
	{
		assert(m_Mutex);
		IMutexLocker theMutex(m_Mutex);
		fRTPStreamObject = NULL;
	};

	int MAX_DELAY_TIME;//延迟最大随机数，该随机数需要从初始化文件读出

public:
	virtual LK_Error Open(unsigned char* /* inData */, unsigned long /* inLength */) {return LK_NoErr;};
	virtual LK_Error Close()  {return LK_NoErr;};
	virtual unsigned long ReceiveData(	HANDLE hSender, Tag inSenderTag, 
		unsigned char* inData, unsigned long inLength, unsigned long inDataType, unsigned long* ioCallbackParam =NULL);

private:
	void QTSS_Write_Error_Proc(QTSS_Error theErr, unsigned long inLength);
	
private:
	static const int WouldBlock_SleepMaxTime = 1000;
	static const int WouldBlock_SleepUnitTime = 50;
	QTSS_Object fRTPStreamObject;	//Used for write data, this object will be deleted in RTPSession::~RTPSession()  when ClientSession closed
	QTSS_Object fRTPSessionObject;

	UInt16 fSequenceNumber;
	BOOL fbMetaDataSented;

	unsigned int fnDelayTime;		//Set the delay time to avoid burst bandwidth
	
	const LPIMutex m_Mutex;
	//int fnVideoQuality;			//Reserved, set the video quality
};

typedef		boost::shared_ptr<ConnectionElem>		LP_S_ConnectionElem;
typedef		std::list<LP_S_ConnectionElem>			Connection_S_List;
typedef		Connection_S_List::iterator				Connection_S_List_iter;