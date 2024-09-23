#pragma once
#include <Windows.h>

namespace SevenZap
{
  enum CompressionLevel
  {
    X1_Fastest = 1,
    X2 = 2,
    X3 = 3,
    X4 = 4,
    X5_Medium = 5,
    X6 = 6,
    X7 = 7,
    X8 = 8,
    X9_Smallest = 9,
  };

  extern "C" HRESULT __stdcall Update7z(const wchar_t* archivePath, HANDLE hCancelEvent, const int numFiles, const wchar_t** files, const wchar_t** entryNames = nullptr, CompressionLevel level = CompressionLevel::X5_Medium);

  extern "C" HRESULT __stdcall Extract7z(const wchar_t* archivePath, const wchar_t* targetFolder);
};
