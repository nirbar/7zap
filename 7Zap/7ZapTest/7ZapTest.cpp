#include "lzma-sdk/CPP/Common/MyInitGuid.h"
#include "pch.h"
#include "lzma-sdk/CPP/Common/MyWindows.h"
#include <Psapi.h>
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
#include "lzma-sdk/CPP/7zip/UI/Common/ArchiveCommandLine.h"
#include "lzma-sdk/CPP/7zip/UI/Common/Bench.h"
#include "lzma-sdk/CPP/7zip/UI/Common/ExitCode.h"
#include "lzma-sdk/CPP/7zip/UI/Common/Extract.h"
#include "lzma-sdk/CPP/7zip/Common/RegisterCodec.h"
#include "lzma-sdk/CPP/7zip/UI/Console/BenchCon.h"
#include "lzma-sdk/CPP/7zip/UI/Console/ConsoleClose.h"
#include "lzma-sdk/CPP/7zip/UI/Console/ExtractCallbackConsole.h"
#include "lzma-sdk/CPP/7zip/UI/Console/HashCon.h"
#include "lzma-sdk/CPP/7zip/UI/Console/List.h"
#include "lzma-sdk/CPP/7zip/UI/Console/OpenCallbackConsole.h"
#include "lzma-sdk/CPP/7zip/MyVersion.h"

#include "lzma-sdk/CPP/7zip/UI/Common/LoadCodecs.h"
#include "lzma-sdk/CPP/7zip/UI/Console/ExtractCallbackConsole.h"
#include "lzma-sdk/CPP/7zip/UI/Common/EnumDirItems.h"
#include "lzma-sdk/CPP/7zip/UI/Common/Extract.h"
#include "lzma-sdk/CPP/7zip/UI/Console/UpdateCallbackConsole.h"
#include "PanelSwLzmaExtractCallback.h"
#include "PanelSwLzmaInStream.h"
#include "PanelSwLzmaOutStream.h"

extern bool g_IsNT;
bool g_IsNT = true;

extern class CStdOutStream* g_ErrStream;
class CStdOutStream* g_ErrStream = new CStdOutStream();

extern HINSTANCE g_hInstance;
HINSTANCE g_hInstance = NULL;

int main(int argc, char** argv)
{
  HRESULT hr = S_OK;
  FString archivePath = L"D:\\Projects\\7Zap\\test.7z";
  FString targetFolder = L"D:\\Projects\\7Zap\\test\\";

  if (argc > 1)
  {
    archivePath = argv[1];
  }
  if (argc > 2)
  {
    targetFolder = argv[2];
  }

  CREATE_CODECS_OBJECT;
  codecs->Load();

  CPanelSwLzmaInStream* inStreamSpec = new CPanelSwLzmaInStream();
  CMyComPtr<IInStream> inStream(inStreamSpec);

  HANDLE hArchive = ::CreateFileW(archivePath, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  hr = inStreamSpec->InitArchive(hArchive);
  ReleaseFileHandle(hArchive);
  if (FAILED(hr))
  {
    return hr;
  }

  std::unique_ptr<CArc> pArc(new CArc());
  COpenOptions openOpts;
  openOpts.codecs = codecs;
  openOpts.stream = inStream;
  openOpts.excludedFormats = new CIntVector();
  openOpts.props = new CObjectVector<CProperty>();
  hr = pArc->OpenStream(openOpts);
  if (FAILED(hr))
  {
    return hr;
  }

  // Print files:
  UInt32 fileCount = 0;
  pArc->Archive->GetNumberOfItems(&fileCount);
  std::unique_ptr<UInt32[]> extractIndices;
  extractIndices.reset(new UInt32[fileCount]);
  std::unique_ptr<FString[]> extractPaths;
  extractPaths.reset(new FString[fileCount]);
  for (UInt32 i = 0; i < fileCount; ++i)
  {
    PROPVARIANT pv = {};

    hr = pArc->Archive->GetProperty(i, kpidIsDir, &pv);
    if (SUCCEEDED(hr) && pv.boolVal)
    {
      printf("Directory: ");
    }

    hr = pArc->Archive->GetProperty(i, kpidPath, &pv);
    if (SUCCEEDED(hr))
    {
      printf("%ls\n", pv.bstrVal);
    }

    extractIndices[i] = i;
    extractPaths[i] = targetFolder;
    extractPaths[i] += pv.bstrVal;
  }

  CPanelSwLzmaExtractCallback* extractClbk = new CPanelSwLzmaExtractCallback();
  CMyComPtr<IArchiveExtractCallback> e = extractClbk;
  e.Detach();
  CExtractNtOptions ntOpts;
  NWildcard::CCensorNode censor;
  censor.Add_Wildcard();
  extractClbk->Init(pArc->Archive, fileCount, extractIndices.get(), extractPaths.get());

  hr = pArc->Archive->Extract(extractIndices.get(), fileCount, 0, extractClbk);

  pArc.reset();

  if (FAILED(hr))
  {
    printf("Failed to extract. Error 0x%08X", hr);
  }
  return hr;
}
