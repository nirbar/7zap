#include "pch.h"
#include "lzma-sdk/CPP/7zip/Archive/7z/7zHandler.h"
#include "lzma-sdk/CPP/7zip/UI/Console/ExtractCallbackConsole.h"
#include "lzma-sdk/CPP/7zip/UI/Console/OpenCallbackConsole.h"
#include "lzma-sdk/CPP/7zip/UI/Common/UpdateCallback.h"
#include "lzma-sdk/CPP/7zip/UI/Common/ArchiveOpenCallback.h"
#include "lzma-sdk/CPP/7zip/UI/Common/EnumDirItems.h"
#include "lzma-sdk/CPP/7zip/UI/Common/Extract.h"

int main(int argc, char* argv)
{
  FChar* archivePath = L"D:\\Projects\\7Zap\\test.7z";
  FChar* targetFolder = L"D:\\Projects\\7Zap\\test";

  HRESULT hr = S_OK;

  CREATE_CODECS_OBJECT;
  codecs->Load();

  CInFileStream* inStreamSpec = new CInFileStream();
  CMyComPtr<IInStream> inStream(inStreamSpec);
  if (!inStreamSpec->Open(archivePath))
  {
    return E_FAIL;
  }

  CArc* pArc = new CArc();
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

  CExtractCallbackConsole* ecs = new CExtractCallbackConsole;
  CMyComPtr<IFolderArchiveExtractCallback> extractCallback = ecs;

  ecs->Init(nullptr, nullptr, nullptr);
  ecs->MultiArcMode = false;

  CArchiveExtractCallback* extractClbk = new CArchiveExtractCallback();
  CExtractNtOptions ntOpts;
  NWildcard::CCensorNode censor;
  censor.Add_Wildcard();
  extractClbk->Init(ntOpts, &censor, pArc, extractCallback, false, false, targetFolder, UStringVector(), false, 0);

  hr = pArc->Archive->Extract(nullptr, -1, 0, extractClbk);
  if (FAILED(hr))
  {
    return hr;
  }
  if (ecs->NumArcsWithError || ecs->NumCantOpenArcs || ecs->NumFileErrors)
  {
    return E_FAIL;
  }
  return hr;
}
