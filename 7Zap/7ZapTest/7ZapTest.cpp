#include "..\7Zap\7Zap.h"
#include <Windows.h>

int wmain(int argc, wchar_t** argv)
{
  HRESULT hr = S_OK;
  wchar_t* archivePath = L"D:\\Projects\\7Zap\\test.7z";
  wchar_t* compressFolder = L"D:\\Projects\\7Zap\\lzma-sdk";
  wchar_t* targetFolder = L"D:\\Projects\\7Zap\\test\\";

  if (argc > 1)
  {
    archivePath = argv[1];
  }
  if (argc > 2)
  {
    compressFolder = argv[2];
  }
  if (argc > 3)
  {
    targetFolder = argv[3];
  }

  const wchar_t* pCompressFolder = &compressFolder[0];
  Update7z(archivePath, NULL, 1, &pCompressFolder, nullptr);

  Extract7z(archivePath, targetFolder);
}
