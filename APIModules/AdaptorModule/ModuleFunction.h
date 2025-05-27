//#include "ModuleFunction.h"		: Used by AdaptorModuleImp.cpp

#if !defined(AFX_MODULEFUNCTION_H__287D738D_86CE_4378_9C2B_BC927C558270__INCLUDED_)
#define AFX_MODULEFUNCTION_H__287D738D_86CE_4378_9C2B_BC927C558270__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StreamingServerFiles.h"

class ResizeableStringFormatter;
class StrPtrLen;

class ModuleFunction  
{
public:
	ModuleFunction();
	virtual ~ModuleFunction();

	static QTSS_Error  Register(QTSS_Register_Params* inParams);
	static QTSS_Error genDoDescribeSDP_ForDoDescribe(ResizeableStringFormatter* inFullSDPBuffer, QTSS_StandardRTSP_Params* inParams);
	static Bool16 isRequestSelfDefined(QTSS_StandardRTSP_Params* inParams);
	static UInt32 WriteSDPHeader(/*FILE* sdpFile, */iovec *theSDPVec, SInt16 *ioVectorIndex, StrPtrLen *sdpHeader);
};

#endif // !defined(AFX_MODULEFUNCTION_H__287D738D_86CE_4378_9C2B_BC927C558270__INCLUDED_)
