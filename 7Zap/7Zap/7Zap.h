#pragma once
#include <Windows.h>

extern "C" HRESULT __stdcall Update7z(const wchar_t * archivePath, HANDLE hCancelEvent, const int numFiles, const wchar_t * *files, const wchar_t** entryNames);

extern "C" HRESULT __stdcall Extract7z(const wchar_t * archivePath, const wchar_t * targetFolder);
