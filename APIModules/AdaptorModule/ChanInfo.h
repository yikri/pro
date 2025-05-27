// #include "ChanInfo.h"

#pragma once
#include "LeKuo.h"

class ChanInfo
{
public:
	ChanInfo(const char* inString);
	~ChanInfo(void);

private:
	void set_IP_Chan_Pair();
public:
	char m_IP[NORM_CHAR_SIZE];
	char m_Port[NORM_CHAR_SIZE];
	char m_IP_Chan_Pair[NORM_CHAR_SIZE];
	char m_Chan[NORM_CHAR_SIZE];
	char m_UserName[NORM_CHAR_SIZE];
	char m_Password[NORM_CHAR_SIZE];
};
