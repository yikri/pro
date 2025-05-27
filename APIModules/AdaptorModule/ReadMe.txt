D:\_D_Bak\rs\zz_test\RelayServer_vs2012_passedBuild_20130323\dev\coding\src\server\AdaptorModule


1. 程序设计主要思路:
	a. ChannelSharedMapping是ModAdaptor和设备之间主要接口，通过m_DeviceManager实现设备的创建和删除
		i. CreateChannel
			1). CreateDVR, LoginDVR
			2). CreateChannel			
		ii. DeleteChannel
			1). Stop Channel, DeleteChannel
			2). LogoutDVR
			3). Destroy
	b. 播放操作直接通过CreateChannel返回的对象
	
	c. ChannelSharedMapping删除前，Channel不再删除
	
2. 关于vHikAdaptor说见 vHikAdaptor
