#include "pch.h"
#include "lzma-sdk/CPP/7zip/UI/Console/ExtractCallbackConsole.h"
#include "lzma-sdk/CPP/7zip/UI/Common/EnumDirItems.h"
#include "lzma-sdk/CPP/7zip/UI/Common/Extract.h"

extern "C" HRESULT __stdcall Extract7z(const FChar * archivePath, const FChar * targetFolder)
{
  HRESULT hr = S_OK;
  UStringVector archivePathsSorted;
  UStringVector archivePathsFullSorted;
  CExtractScanConsole scan;
  CDirItemsStat st;
  NWildcard::CCensor arcCensor;
  NWildcard::CCensorNode censor;
  NWildcard::CCensorPathProps censorProps;

  //Init
  censor.Add_Wildcard();
  scan.Init(nullptr, nullptr, nullptr);
  censorProps.WildcardMatching = DoesNameContainWildcard(archivePath);
  censorProps.Recursive = censorProps.WildcardMatching;
  censorProps.MarkMode = NWildcard::kMark_FileOrDir;
  arcCensor.AddItem(NWildcard::ECensorPathMode::k_RelatPath, true, archivePath, censorProps);

  // Scan
  scan.StartScanning();
  hr = EnumerateDirItemsAndSort(arcCensor, NWildcard::k_RelatPath, targetFolder, archivePathsSorted, archivePathsFullSorted, st, &scan);

  scan.CloseScanning();
  if (hr != S_OK)
  {
    return hr;
  }

  CExtractCallbackConsole* ecs = new CExtractCallbackConsole;
  CMyComPtr<IFolderArchiveExtractCallback> extractCallback = ecs;

  ecs->Init(nullptr, nullptr, nullptr);
  ecs->MultiArcMode = (archivePathsSorted.Size() > 1);

  CExtractOptions eo;
  eo.YesToAll = true;
  eo.OutputDir = targetFolder;

  UString errorMessage;
  CDecompressStat stat;
  CObjectVector<COpenType> types;
  CIntVector excludedFormats;

  CREATE_CODECS_OBJECT;
  codecs->Load();

  hr = Extract(codecs, types, excludedFormats, archivePathsSorted, archivePathsFullSorted, censor, eo, ecs, ecs, ecs, nullptr, errorMessage, stat);
  ecs->ClosePercents();

  if (!errorMessage.IsEmpty())
  {
    if (hr == S_OK)
    {
      hr = E_FAIL;
    }
    return hr;
  }

  if (ecs->NumCantOpenArcs || ecs->NumArcsWithError || ecs->NumOpenArcErrors)
  {
    return E_FAIL;
  }
  return S_OK;
}
