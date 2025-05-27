// #include "ChannelElem_4_Channel_S_map.h"

#pragma once

#include "lkRTPPacketFormat.h"
#include "StdRsType.h"
#include "RsBoostType.h"
#include "ConnectionElem.h"
#include "StreamPackager.h"

class ChannelElem_4_Channel_S_map : public IDataReceiver// : public IChannel
{
public:
	//ChannelElem_4_Channel_S_map(boost::shared_ptr<DeviceManager> inDeviceManager, const LPIDVR inDVR, const char* inCreateStr, Tag inTag, const LPIMutex inMutex);
	ChannelElem_4_Channel_S_map(const LPIDVR inDVR, const char* inCreateStr, Tag inTag, const LPIMutex inMutex);
	~ChannelElem_4_Channel_S_map(void);

	virtual LK_Error Attach_DataReceiver(LP_S_ConnectionElem inConnection);
	LK_Error Detach_DataReceiver(LP_S_ConnectionElem inConnection);

	virtual LK_Error Play(const char* inPlayOption);
	virtual BOOL IsPlaying();
	virtual LK_Error Stop();
	//virtual LK_Error Pause();

	virtual MetadataLen getMetadataLen();
	virtual MetadataLen getMetadata(unsigned char* outDataBuffer, INT32 inBufferLength);


	virtual unsigned long ReceiveData(	HANDLE hSender, Tag inSenderTag,
		unsigned char* inData, 	unsigned long inLength, unsigned long inDataType, unsigned long* outReceiveResult=NULL);

	unsigned long SendMetaData( LP_S_ConnectionElem theConnectElem, unsigned long inDataType, unsigned char* inData, unsigned long inLength, HANDLE hSender, Tag inSenderTag, unsigned long* ioCallbackParam );

public:
	rsString getCreateStr() {return m_ChannelCreateStr;};
private:
	// Print the Call back parameter 
	void ProcessData_PrintPara(HANDLE hSender, Tag inSenderTag,	unsigned char* inData, unsigned long inLength, unsigned long inDataType, unsigned long* ioCallbackParam);

private:
	//boost::shared_ptr<DeviceManager> m_pDeviceManager;
	const LPIDVR			m_pDVR;				//Passed from ChannelSharedMapping
	const rsString			m_ChannelCreateStr;
	Tag m_Tag;
	const LPIMutex m_Mutex;

private:
	//boost::scoped_ptr<IChannel> m_Channel;
	LPIChannel m_Channel;
	Connection_S_List m_ConnectionList;
	StreamPackager	m_Packager;
};

#pragma region STL_BOOST types

typedef		boost::shared_ptr<IDVR>				LP_S_IDVR;
typedef		std::map<rsString, LP_S_IDVR>		DVR_S_map;
typedef		DVR_S_map::iterator					DVR_S_map_iter;

typedef		ChannelElem_4_Channel_S_map*					LPChannelElem;
typedef		boost::shared_ptr<ChannelElem_4_Channel_S_map>	LP_S_ChannelElem;
typedef		std::map<rsString, LP_S_ChannelElem>			Channel_S_map;
typedef		Channel_S_map::iterator							Channel_S_map_iter;

#pragma endregion STL_BOOST types