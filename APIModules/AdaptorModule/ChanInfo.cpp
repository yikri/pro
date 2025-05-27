#include "ChanInfo.h"
#include "RsTokenParser.h"
#include "StdRsType.h"

ChanInfo::ChanInfo(const char* inString)
{
	RsTokenParser theTokenParser(inString);
	theTokenParser.getToken(LK_KEYWORD_STR[LK_KEYWORD_Device_IP], m_IP, NORM_CHAR_SIZE, NULL);
	theTokenParser.getToken(LK_KEYWORD_STR[LK_KEYWORD_Device_Port], m_Port, NORM_CHAR_SIZE, NULL);
	theTokenParser.getToken(LK_KEYWORD_STR[LK_KEYWORD_Device_UserName], m_UserName, NORM_CHAR_SIZE, NULL);
	theTokenParser.getToken(LK_KEYWORD_STR[LK_KEYWORD_Device_Password], m_Password, NORM_CHAR_SIZE, NULL);

	theTokenParser.getToken(LK_KEYWORD_STR[LK_KEYWORD_Channel_ID], m_Chan, NORM_CHAR_SIZE, NULL);

	this->set_IP_Chan_Pair();
}


void ChanInfo::set_IP_Chan_Pair()
{
	strcpy_s(m_IP_Chan_Pair, NORM_CHAR_SIZE, m_IP); 
	strcat_s(m_IP_Chan_Pair, NORM_CHAR_SIZE, ":");
	strcat_s(m_IP_Chan_Pair, NORM_CHAR_SIZE, m_Chan);
}

ChanInfo::~ChanInfo(void)
{
}
