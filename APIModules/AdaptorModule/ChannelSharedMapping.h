// #include "ChannelSharedMapping.h"
#pragma once
#include "RsBoostType.h"
#include "ChannelElem_4_Channel_S_map.h"

class ChannelSharedMapping
{
public:
	ChannelSharedMapping(const LPIMutex inMutex);
	~ChannelSharedMapping(void);

	LP_S_ChannelElem CreateChannel(rsString inFullString);
	//void DestroyChannel(rsString inIP_ChanStr);				//�Ƿ���Ҫɾ��Channel? ���Բ�ɾ������Ϊ�ǹ���������Ըö����һֱ���ڣ�ֻ����ϵͳ�˳�ʱ��ɾ��
	//void DestroyChannel(LP_S_ChannelElem inChannel);

	//void DestroyMapping();

private:
	//boost::shared_ptr<DeviceManager> m_DeviceManager;
	DVR_S_map		m_DVR_S_map;		//DVRIP as the Key
	Channel_S_map	m_Channel_S_map;	//DVRIP:ChanID as the Key
	enum{ 
		m_DVRTag = 0,
		m_ChannelTag = 0,
	};

private:
	const LPIMutex m_Mutex;
};

