#include "pch.h"
#include "lzma-sdk/CPP/7zip/Common/FileStreams.h"
#include "lzma-sdk/CPP/7zip/Archive/IArchive.h"
#include "PanelSwLzmaExtractCallback.h"
#include "PanelSwLzmaOutStream.h"

HRESULT CPanelSwLzmaExtractCallback::Init(IInArchive* archive, UInt32 extractCount, UInt32* extractIndices, FString* extractPaths)
{
	HRESULT hr = S_OK;

	_archive = archive;

	_extractIndices.reset(new UInt32[extractCount]);
  if (!_extractIndices.get())
  {
    hr = E_OUTOFMEMORY;
    printf("Failed to allocate memory. Error 0x%08X", ::GetLastError());
    goto LExit;
  }

  _extractPaths.reset(new FString[extractCount]);
  if (!_extractPaths.get())
  {
    hr = E_OUTOFMEMORY;
    printf("Failed to allocate memory. Error 0x%08X", ::GetLastError());
    goto LExit;
  }

	_dwErrCount = 0;
	_extractCount = extractCount;
	for (UInt32 i = 0; i < _extractCount; ++i)
	{
		_extractIndices[i] = extractIndices[i];
		_extractPaths[i] = extractPaths[i];
	}

LExit:
	return hr;
}


Z7_COM7F_IMF(CPanelSwLzmaExtractCallback::GetStream(UInt32 index, ISequentialOutStream** outStream, Int32 askExtractMode))
{
  HRESULT hr = S_OK;
  BOOL bRes = TRUE;
  CPanelSwLzmaOutStream* os = nullptr;
  CMyComPtr<ISequentialOutStream> outOs;

  if (outStream != nullptr)
  {
    *outStream = nullptr;

    if (askExtractMode == NArchive::NExtract::NAskMode::kExtract)
    {
      for (UInt32 i = 0; i < _extractCount; ++i)
      {
        if (_extractIndices[i] == index)
        {
          PROPVARIANT pv = {};
          FILETIME ftCreate = { 0,0 };
          FILETIME ftAccess = { 0,0 };
          FILETIME ftWrite = { 0,0 };
          UInt64 ullSize = 0;

          if (SUCCEEDED(_archive->GetProperty(index, kpidCTime, &pv)) && (pv.vt == VT_FILETIME))
          {
            ftCreate.dwLowDateTime = pv.filetime.dwLowDateTime;
            ftCreate.dwHighDateTime = pv.filetime.dwHighDateTime;
          }
          if (SUCCEEDED(_archive->GetProperty(index, kpidATime, &pv)) && (pv.vt == VT_FILETIME))
          {
            ftAccess.dwLowDateTime = pv.filetime.dwLowDateTime;
            ftAccess.dwHighDateTime = pv.filetime.dwHighDateTime;
          }
          if (SUCCEEDED(_archive->GetProperty(index, kpidMTime, &pv)) && (pv.vt == VT_FILETIME))
          {
            ftWrite.dwLowDateTime = pv.filetime.dwLowDateTime;
            ftWrite.dwHighDateTime = pv.filetime.dwHighDateTime;
          }

          os = new CPanelSwLzmaOutStream();
          if (!os)
          {
            hr = E_OUTOFMEMORY;
            printf("Failed to allocate memory. Error 0x%08X", ::GetLastError());
            goto LExit;
          }

          hr = os->Create(_extractPaths[i], ftCreate, ftAccess, ftWrite);
          if (FAILED(hr))
          {
            printf("Failed to create file '%ls'. Error 0x%08X", (const wchar_t*)_extractPaths[i], hr);
            goto LExit;
          }

          if (SUCCEEDED(_archive->GetProperty(index, kpidSize, &pv)))
          {
            ullSize = (pv.vt == VT_UI8) ? pv.uhVal.QuadPart : (pv.vt == VT_I8) ? pv.hVal.QuadPart : (pv.vt == VT_UI4) ? pv.uintVal : (pv.vt == VT_I4) ? pv.intVal : 0;
            if (ullSize > 0)
            {
              ((IOutStream*)os)->SetSize(ullSize);
            }
          }

          outOs = os;
          os = nullptr;
          *outStream = outOs.Detach();

          _extractIndices[i] = INFINITE; // Allow the same index to be extracted to different targets
          break;
        }
      }
    }
  }

LExit:
  if (os)
  {
    delete os;
  }
  if (SUCCEEDED(hr) && _dwErrCount)
  {
    hr = E_FAIL;
  }

  return bRes ? hr : E_FAIL;
}

Z7_COM7F_IMF(CPanelSwLzmaExtractCallback::PrepareOperation(Int32 askExtractMode))
{
	return _dwErrCount ? E_FAIL : S_OK;
}

Z7_COM7F_IMF(CPanelSwLzmaExtractCallback::SetOperationResult(Int32 opRes))
{
	HRESULT hr = S_OK;
	LPCWSTR szErr = nullptr;

  if (opRes != NArchive::NExtract::NOperationResult::kOK)
  {
    ++_dwErrCount;
    hr = HRESULT_FROM_WIN32(opRes);

    OperationResultToString(opRes, &szErr);
    hr = E_OUTOFMEMORY;
    printf("Failed to allocate memory. Error 0x%08X. %ls", ::GetLastError(), szErr);
    goto LExit;
  }

LExit:

	return FAILED(hr) ? hr : _dwErrCount ? E_FAIL : S_OK;
}

Z7_COM7F_IMF(CPanelSwLzmaExtractCallback::ReportExtractResult(UInt32 indexType, UInt32 index, Int32 opRes))
{
	HRESULT hr = S_OK;
	LPCWSTR szErr = nullptr;

	if (opRes != NArchive::NExtract::NOperationResult::kOK)
	{
		++_dwErrCount;
		hr = HRESULT_FROM_WIN32(opRes);

		OperationResultToString(opRes, &szErr);
    printf("Failed to extract. %ls", szErr);
	}

LExit:
	
	return FAILED(hr) ? hr : _dwErrCount ? E_FAIL : S_OK;
}

HRESULT CPanelSwLzmaExtractCallback::OperationResultToString(Int32 opRes, LPCWSTR* psz) const
{
  HRESULT hr = S_OK;
  static const wchar_t* kOK = L"kOk";
  static const wchar_t* kUnsupportedMethod = L"kUnsupportedMethod";
  static const wchar_t* kDataError = L"kDataError";
  static const wchar_t* kCRCError = L"kCRCError";
  static const wchar_t* kUnavailable = L"kUnavailable";
  static const wchar_t* kUnexpectedEnd = L"kUnexpectedEnd";
  static const wchar_t* kDataAfterEnd = L"kDataAfterEnd";
  static const wchar_t* kIsNotArc = L"kIsNotArc";
  static const wchar_t* kHeadersError = L"kHeadersError";
  static const wchar_t* kWrongPassword = L"kWrongPassword";
  static const wchar_t* kUnknown = L"kUnknown";

  switch (opRes)
  {
  case NArchive::NExtract::NOperationResult::kOK:
    *psz = kOK;
    break;
  case NArchive::NExtract::NOperationResult::kUnsupportedMethod:
    *psz = kUnsupportedMethod;
    break;
  case NArchive::NExtract::NOperationResult::kDataError:
    *psz = kDataError;
    break;
  case NArchive::NExtract::NOperationResult::kCRCError:
    *psz = kCRCError;
    break;
  case NArchive::NExtract::NOperationResult::kUnavailable:
    *psz = kUnavailable;
    break;
  case NArchive::NExtract::NOperationResult::kUnexpectedEnd:
    *psz = kUnexpectedEnd;
    break;
  case NArchive::NExtract::NOperationResult::kDataAfterEnd:
    *psz = kDataAfterEnd;
    break;
  case NArchive::NExtract::NOperationResult::kIsNotArc:
    *psz = kIsNotArc;
    break;
  case NArchive::NExtract::NOperationResult::kHeadersError:
    *psz = kHeadersError;
    break;
  case NArchive::NExtract::NOperationResult::kWrongPassword:
    *psz = kWrongPassword;
    break;
  default:
    *psz = kUnknown;
    break;
  }

LExit:
  return hr;
}


Z7_COM7F_IMF(CPanelSwLzmaExtractCallback::SetCompleted(const UInt64* total))
{
	return _dwErrCount ? E_FAIL : S_OK;
}

Z7_COM7F_IMF(CPanelSwLzmaExtractCallback::SetTotal(UInt64 total))
{
	return _dwErrCount ? E_FAIL : S_OK;
}

BOOL CPanelSwLzmaExtractCallback::HasErrors() const
{
	return _dwErrCount ? TRUE : FALSE;
}
