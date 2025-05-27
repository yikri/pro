// ServerControl.cpp: implementation of the CServerControl class.
//
//////////////////////////////////////////////////////////////////////

#include "ServerControl.h"
#include "IniReader2.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CServerControl::CServerControl()
{

}

CServerControl::~CServerControl()
{

}

SERVER_CONTROL_STATUS CServerControl::getServerControlAndReset()
{
	SERVER_CONTROL_STATUS theStatus = CServerControl::getServerControl();
	if ( SERVER_CONTROL_STATUS_STOP == theStatus)
	{
		CServerControl::setServerControl(SERVER_CONTROL_STATUS_RUN);
	}
	return theStatus;
}


SERVER_CONTROL_STATUS CServerControl::getServerControl()	//��÷��������ƣ��Ƿ�Ҫֹͣ�����������0����ʾҪֹͣ�����
{
	int theNStatus;
	CIniReader2 theReader("VideoRelay.ini");
	theNStatus = theReader.ReadInteger("RunServer", "SERVER_CONTROL_STATUS",SERVER_CONTROL_STATUS_RUN);
	return (SERVER_CONTROL_STATUS)theNStatus;

}
	//
void CServerControl::setServerControl(SERVER_CONTROL_STATUS inStatus)
{
	CIniReader2 theReader("VideoRelay.ini");
	theReader.WriteInteger("RunServer", "SERVER_CONTROL_STATUS", inStatus);
}
