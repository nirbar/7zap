#ifndef PCH_7ZAPTEST_H_
#define PCH_7ZAPTEST_H_

#include "lzma-sdk/CPP/Common/Common.h"
#include "lzma-sdk/CPP/Common/MyWindows.h"
#include "lzma-sdk/C/CpuArch.h"

#include "lzma-sdk/CPP/Common/CommandLineParser.h"
#include "lzma-sdk/CPP/Common/IntToString.h"
#include "lzma-sdk/CPP/Common/MyException.h"
#include "lzma-sdk/CPP/Common/StdInStream.h"
#include "lzma-sdk/CPP/Common/StdOutStream.h"
#include "lzma-sdk/CPP/Common/StringConvert.h"
#include "lzma-sdk/CPP/Common/StringToInt.h"
#include "lzma-sdk/CPP/Common/UTFConvert.h"
#include "lzma-sdk/CPP/Windows/ErrorMsg.h"
#include "lzma-sdk/CPP/Windows/TimeUtils.h"
#include "lzma-sdk/CPP/Windows/FileDir.h"
#include "WixLogMacros.h"

typedef long HRESULT;

#ifndef FACILITY_7ZAP
#define FACILITY_7ZAP 600
#endif

#ifndef MAKE_HRESULT
#define MAKE_HRESULT(sev,fac,code) \
    ((HRESULT) (((unsigned long)(sev)<<31) | ((unsigned long)(fac)<<16) | ((unsigned long)(code))) )
#endif

#ifndef HRESULT_FROM_7ZAP
#define HRESULT_FROM_7ZAP(e)	MAKE_HRESULT(1, FACILITY_7ZAP, e)
#endif

#endif
