#pragma once
#include "lzma-sdk/CPP/7zip/UI/Console/UpdateCallbackConsole.h"
#include <list>

class CUpdateCallback7Zap :
  public CUpdateCallbackConsole
{
public:

  CUpdateCallback7Zap(HANDLE hCancelEvent = NULL, size_t nFiles = 0, const FChar** ppFiles = nullptr, const wchar_t** ppEntryNames = nullptr);

  virtual ~CUpdateCallback7Zap();

  HRESULT CheckBreak() Z7_override;

  HRESULT OverrideLogName(const FString& physPath, UString& logPath, bool isDir) Z7_override;

private:

  bool ArePathsEqual(const FString& path1, const FString& path2) const;

  bool IsNested(const FString& parent, const FString& child) const;

  HANDLE _hCancelEvent = NULL;
  FString* _pFiles = nullptr;
  UString* _pEntryNames = nullptr;
  size_t _nFiles = 0;

  struct FolderLogName
  {
    FString physPath;
    UString ovrdLogName;
  };

  std::list<FolderLogName> _folders;
  typedef std::list<FolderLogName>::const_iterator FoldersMapIterator;
};
