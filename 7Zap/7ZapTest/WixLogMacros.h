#pragma once

#define BextLog(l, f, ...)                          printf(f "\n", __VA_ARGS__)
#define BextLogError(e, f, ...)                     printf("ERROR 0x%08X. " f "\n", e, __VA_ARGS__)
#define BextExitOnFailure(h, f, ...)                if(FAILED(h)) { printf("ERROR 0x%08X. " f "\n", h, __VA_ARGS__); goto LExit; }
#define BextExitOnNullWithLastError(p, h, f, ...)   if(!(p)) { h = HRESULT_FROM_WIN32(::GetLastError()); printf("ERROR 0x%08X. " f "\n", h, __VA_ARGS__); goto LExit; }
#define BextExitOnNull(p, h, e, f, ...)             if(!(p)) { h = e; printf("ERROR 0x%08X. " f "\n", h, __VA_ARGS__); goto LExit; }
#define BextExitOnWin32Error(e, h,  f, ...)         if(e) { h = HRESULT_FROM_WIN32(e); printf("ERROR 0x%08X. " f "\n", h, __VA_ARGS__); goto LExit; }

#define ReleaseHandle(h)                  if(h) { ::CloseHandle(h); h = NULL; }
#define ReleaseFileHandle(h)              ::CloseHandle(h); h = INVALID_HANDLE_VALUE;
#define ReleaseStr(s)                     SysFreeString(s)
#define ReleaseNullStr(s)                 SysFreeString(s); s = nullptr;
#define ReleaseNullMem(p)                 free(p); p = nullptr;

#define MemAlloc(l, z)                    malloc(l)
#define StrAllocString(ps, s, l)          S_OK; *ps = ::SysAllocString(s)
#define StrAllocFormatted(ps, f, ...)     S_OK; *ps = SysAllocStringByteLen(nullptr, 1000); ::swprintf(*ps, f, __VA_ARGS__);

#define InitArchive(h)                    InitContainer(h, 0, CPanelSwLzmaInStream::INFINITE_CONTAINER_SIZE)

#define ExitFunction()                    goto LExit;
