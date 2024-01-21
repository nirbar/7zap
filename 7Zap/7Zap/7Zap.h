#pragma once
#include "pch.h"

extern "C" HRESULT __stdcall Update7z(const FChar * archivePath, HANDLE hCancelEvent, const int numFiles, const FChar * *files, const wchar_t** entryNames);

extern "C" HRESULT __stdcall Extract7z(const FChar * archivePath, const FChar * targetFolder);
