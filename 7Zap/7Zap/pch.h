#pragma once

#include "lzma-sdk/CPP/Common/Common.h"
#include "lzma-sdk/CPP/Common/MyString.h"

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
