
#include "ChannelSharedMapping.h"
#include "RsTokenParser.h"
#include "ChanInfo.h"
#include "StdRsType.h"

#include "dvrAdaptor.h"

ChannelSharedMapping::ChannelSharedMapping(const LPIMutex inMutex) : m_Mutex(inMutex)
{
	//m_DeviceManager.reset(new DeviceManager(DeviceManager_Types_Server));
	//m_DeviceManager->LoadDevice(m_Mutex);
	char theSetupStr[MAX_CHAR_SIZE];
	sprintf_s(theSetupStr, MAX_CHAR_SIZE, "%s=%s", 
		LK_DEVICE_MANAGER_CallerType_STR[LK_DEVICE_MANAGER_CallerType_TypeName],
		LK_DEVICE_MANAGER_CallerType_STR[LK_DEVICE_MANAGER_CallerType_Server]
		);

	Setup(m_Mutex, theSetupStr);
}


ChannelSharedMapping::~ChannelSharedMapping(void)
{
	{
		IMutexLocker theLocker(m_Mutex);

		for(Channel_S_map_iter ChanPos = m_Channel_S_map.begin(); ChanPos != m_Channel_S_map.end(); ++ChanPos)
		{
			ChanPos->second->Stop();
			ChanPos->second.reset();
		}

		for(DVR_S_map_iter DVRPos = m_DVR_S_map.begin(); DVRPos != m_DVR_S_map.end(); ++DVRPos)
		{
			DVRPos->second->Logout();
			DVRPos->second.reset();
		}

		m_Channel_S_map.clear();
		m_DVR_S_map.clear();
	}
	//Unlock here to let the device thread to Exit
	LK_Error theError = Teardown();
	theError;
}


//Create and Login to DVR, Create Channel with Interface IChannel
LP_S_ChannelElem ChannelSharedMapping::CreateChannel(rsString inFullString)
{
	IMutexLocker theLocker(m_Mutex);

	//  Get the Channel Info
	RsTokenParser theTokenParser(inFullString.c_str());
	theTokenParser.DelServerToken();
	char theChannelStr[MAX_CHAR_SIZE];
	theTokenParser.getAllTokens(theChannelStr, MAX_CHAR_SIZE);
	
	ChanInfo theChanInfo(theChannelStr);
	//
	//1. Create and Login to DVR
	rsString theIP = theChanInfo.m_IP;
	LP_S_IDVR the_S_IDVR;
	DVR_S_map_iter theDVRMapPos = m_DVR_S_map.find(theIP);
	if(theDVRMapPos!=m_DVR_S_map.end())
		the_S_IDVR = theDVRMapPos->second;

	if(!the_S_IDVR.get())
	{
		LPIDVR theIDVR = CreateDVR(theChannelStr, m_DVRTag, m_Mutex);
		assert(theIDVR);

		the_S_IDVR = LP_S_IDVR(theIDVR);
		m_DVR_S_map[theIP] = the_S_IDVR;
	}

	assert(the_S_IDVR.get());
	the_S_IDVR->Login(theChanInfo.m_IP, atoi(theChanInfo.m_Port), theChanInfo.m_Password, theChanInfo.m_Password);

	//
	//2. Create the Channel
	LP_S_ChannelElem the_S_IChannel;
	Channel_S_map_iter theChannelMapPos = m_Channel_S_map.find(theChanInfo.m_IP_Chan_Pair);
	if(theChannelMapPos!=m_Channel_S_map.end())
		the_S_IChannel = theChannelMapPos->second;

	if(!the_S_IChannel.get())
	{
		LPChannelElem theChannelElem = new ChannelElem_4_Channel_S_map(the_S_IDVR.get(), theChannelStr, m_ChannelTag, m_Mutex) ;
		assert(theChannelElem);

		rsString theIP_PortPair = theChanInfo.m_IP_Chan_Pair;
		the_S_IChannel = LP_S_ChannelElem(theChannelElem);
		m_Channel_S_map[theIP_PortPair] = the_S_IChannel;
	}
	assert(the_S_IChannel.get());

	return the_S_IChannel;	
}

