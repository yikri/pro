// ServerControl.h: interface for the CServerControl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERVERCONTROL_H__371A7554_C5AB_48D0_B176_7A16906B1694__INCLUDED_)
#define AFX_SERVERCONTROL_H__371A7554_C5AB_48D0_B176_7A16906B1694__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum SERVER_CONTROL_STATUS 
{
	SERVER_CONTROL_STATUS_STOP = 0,
	SERVER_CONTROL_STATUS_RUN =1,
};


class CServerControl  
{
public:
	CServerControl();
	virtual ~CServerControl();
	//
	static SERVER_CONTROL_STATUS getServerControlAndReset();	
private:
	static SERVER_CONTROL_STATUS getServerControl();	//获得服务器控制，是否要停止服务器，如果0，表示要停止服务回
	//
	static void setServerControl(SERVER_CONTROL_STATUS inStatus);
};

#endif // !defined(AFX_SERVERCONTROL_H__371A7554_C5AB_48D0_B176_7A16906B1694__INCLUDED_)
