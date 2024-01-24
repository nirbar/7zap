#include "pch.h"
#include "UpdateCallback7Zap.h"

CUpdateCallback7Zap::CUpdateCallback7Zap(HANDLE hCancelEvent, size_t nFiles, const FChar** ppFiles, const wchar_t** ppEntryNames)
  : _hCancelEvent(hCancelEvent)
{
  Init(nullptr, nullptr, nullptr);

  if (ppFiles && ppEntryNames && nFiles)
  {
    _pFiles = new FString[nFiles];
    _pEntryNames = new UString[nFiles];
    _nFiles = nFiles;

    for (size_t i = 0; i < _nFiles; ++i)
    {
      _pFiles[i] = ppFiles[i];
      if (ppEntryNames[i] && *ppEntryNames[i])
      {
        _pEntryNames[i] = ppEntryNames[i];
      }
    }
  }
}

CUpdateCallback7Zap::~CUpdateCallback7Zap()
{
  if (_pFiles)
  {
    delete[] _pFiles;
    _pFiles = nullptr;
  }
  if (_pEntryNames)
  {
    delete[] _pEntryNames;
    _pEntryNames = nullptr;
  }
  _hCancelEvent = NULL;
  _nFiles = 0;
}

HRESULT CUpdateCallback7Zap::CheckBreak()
{
	return ((_hCancelEvent != NULL) && (_hCancelEvent != INVALID_HANDLE_VALUE) && (::WaitForSingleObject(_hCancelEvent, 0) == WAIT_OBJECT_0)) ? E_ABORT : S_OK;
}

HRESULT CUpdateCallback7Zap::OverrideLogName(const FString& physPath, UString& logPath, bool isDir)
{
  for (size_t i = 0; i < _nFiles; ++i)
  {
    if (!_pFiles[i].IsEmpty() && !_pEntryNames[i].IsEmpty() && ArePathsEqual(_pFiles[i], physPath))
    {
      logPath = _pEntryNames[i];
      if (isDir)
      {
        FolderLogName folder;
        folder.physPath = physPath;
        folder.ovrdLogName = logPath;

        _folders.push_back(folder);
      }

      return S_OK;
    }
  }

  // Find deepest parent folder that had been overriden
  FoldersMapIterator endIt = _folders.end();
  FoldersMapIterator bestMatch = endIt;
  for (FoldersMapIterator it = _folders.begin(); it != endIt; ++it)
  {
    if ((physPath.Find(it->physPath) == 0) && ((bestMatch == endIt) || (bestMatch->physPath.Len() < it->physPath.Len())))
    {
      bestMatch = it;
    }
  }

  // If the physical path is within an already matched folder, replace the prefix
  if (bestMatch != endIt)
  {
    const FChar* postfix = physPath.Ptr(bestMatch->physPath.Len());
    logPath = bestMatch->ovrdLogName + UString(postfix);

    return S_OK;
  }

  return S_FALSE;
}

bool CUpdateCallback7Zap::ArePathsEqual(const FString& path1, const FString& path2)
{
#ifdef _WIN32
  return (wcsicmp((const wchar_t*)path1, (const wchar_t*)path2) == 0);
#else
  return (strcmp((const char*)path1, (const char*)path2) == 0);
#endif // _WIN32
}
