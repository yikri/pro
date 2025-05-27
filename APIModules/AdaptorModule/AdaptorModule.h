
/************************************************************************/
/*  Created by: WuZengde, 2007/07/29									*/
/*  Email: Wuzengde@vip.sina.com										*/
/*																		*/
/************************************************************************/

#ifndef __QTSS_TEST_DEV_MODULE_H__
#define __QTSS_TEST_DEV_MODULE_H__

//#include "QTSS.h"
#include "StreamingServerFiles.h"

extern "C"
{
#ifdef _DEBUG	
    EXPORT QTSS_Error server_AdaptorModuleD_Main(void* inPrivateArgs);
#else
    EXPORT QTSS_Error server_AdaptorModule_Main(void* inPrivateArgs);
#endif
}

#endif // __QTSS_TEST_DEV_MODULE_H__


