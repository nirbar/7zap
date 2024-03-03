#include "pch.h"
#include "PanelSwLzmaOutStream.h"
#include <stdio.h>

CPanelSwLzmaOutStream::~CPanelSwLzmaOutStream()
{
	Close();
}

HRESULT CPanelSwLzmaOutStream::Close()
{
  HRESULT hr = S_OK;
  BOOL bRes = TRUE;
  DWORD dwRes = ERROR_SUCCESS;

  hr = CompleteWrite();
  if (FAILED(hr))
  {
    printf("Failed to write");
    goto LExit;
  }

  if (_hFile != INVALID_HANDLE_VALUE)
  {
    const FILETIME* pftCreate = (_ftCreationTime.dwLowDateTime || _ftCreationTime.dwHighDateTime) ? &_ftCreationTime : nullptr;
    const FILETIME* pftAccess = (_ftLastAccessTime.dwLowDateTime || _ftLastAccessTime.dwHighDateTime) ? &_ftLastAccessTime : nullptr;
    const FILETIME* pftWrite = (_ftLastWriteTime.dwLowDateTime || _ftLastWriteTime.dwHighDateTime) ? &_ftLastWriteTime : nullptr;

    if (pftCreate || pftAccess || pftWrite)
    {
      bRes = FALSE;
      for (unsigned i = 0; !bRes && (i < MAX_RETRIES); ++i)
      {
        bRes = ::SetFileTime(_hFile, pftCreate, pftAccess, pftWrite);
        if (!bRes)
        {
          printf("Failed to set file times for '%ls' on attempt %u/%u. Error 0x%08X", (LPCWSTR)_szPath, i, MAX_RETRIES, ::GetLastError());
          continue;
        }
        break;
      }
      if (!bRes)
      {
        hr = E_FAIL;
        printf("Failed to set file times for '%ls'", (LPCWSTR)_szPath);
        goto LExit;
      }
    }
  }

LExit:
  if (_hFile != INVALID_HANDLE_VALUE)
  {
    CloseHandle(_hFile);
    _hFile = INVALID_HANDLE_VALUE;
  }
  if (_pWriteData)
  {
    delete[]_pWriteData;
    _pWriteData = nullptr;
  }
  _dwWriteAttempts = 0;
  _ullBufferSize = 0;
  _ullWriteSize = 0;
  _ullNextWritePos.QuadPart = 0;
  _szPath.Empty();
  _ftCreationTime = { 0,0 };
  _ftLastAccessTime = { 0,0 };
  _ftLastWriteTime = { 0,0 };

  return bRes ? hr : E_FAIL;
}

HRESULT CPanelSwLzmaOutStream::Create(LPCWSTR szPath, const FILETIME ftCreationTime, const FILETIME ftLastAccessTime, const FILETIME ftLastWriteTime)
{
	BOOL bRes = TRUE;
	HRESULT hr = S_OK;
	DWORD dwAttempts = 0;

	Close();

	_ftCreationTime = ftCreationTime;
	_ftLastAccessTime = ftLastAccessTime;
	_ftLastWriteTime = ftLastWriteTime;

  _szPath = szPath;

	for (unsigned i = 0; (_hFile == INVALID_HANDLE_VALUE) && (i < MAX_RETRIES); ++i)
	{
    _hFile = ::CreateFileW(szPath, GENERIC_ALL, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
		if (_hFile == INVALID_HANDLE_VALUE)
		{
			printf("Failed to create file '%ls' on attempt %u/%u. Error 0x%08X", szPath, i, MAX_RETRIES, ::GetLastError());
      continue;
		}
    break;
	}
  if (_hFile == INVALID_HANDLE_VALUE)
  {
    hr = E_FAIL;
    printf("Failed to create file '%ls'", szPath);
    goto LExit;
  }

LExit:
	if (FAILED(hr))
	{
		Close();
	}
	return hr;
}

Z7_COM7F_IMF(CPanelSwLzmaOutStream::Seek(Int64 offset, UInt32 seekOrigin, UInt64* newPosition))
{
	BOOL bRes = FALSE;
	HRESULT hr = S_OK;
	LARGE_INTEGER liPos = { 0 };
	LARGE_INTEGER liNewPos = { 0 };

	switch (seekOrigin)
	{
	default:
	case ESzSeek::SZ_SEEK_SET:
		seekOrigin = FILE_BEGIN;
		break;
	case ESzSeek::SZ_SEEK_CUR:
		seekOrigin = FILE_CURRENT;
		break;
	case ESzSeek::SZ_SEEK_END:
		seekOrigin = FILE_END;
		break;
	}

	bRes = FALSE;
  liPos.QuadPart = offset; //TODO Copy to WiX
	for (unsigned i = 0; !bRes && (i < MAX_RETRIES); ++i)
	{
		bRes = ::SetFilePointerEx(_hFile, liPos, &liNewPos, seekOrigin);
		if (!bRes)
		{
			printf("Failed to seek in file '%ls' on attempt %u/%u. Error 0x%08X", (LPCWSTR)_szPath, i, MAX_RETRIES, ::GetLastError());
      continue;
		}
    break;
	}
  if (!bRes)
  {
    hr = E_FAIL;
    printf("Failed to seek in file '%ls'", (LPCWSTR)_szPath);
    goto LExit;
  }
  
	if (newPosition)
	{
		*newPosition = liNewPos.QuadPart;
	}
  _ullNextWritePos.QuadPart = liNewPos.QuadPart;

LExit:
  return bRes ? hr : E_FAIL;
}

Z7_COM7F_IMF(CPanelSwLzmaOutStream::SetSize(UInt64 newSize))
{
	BOOL bRes = FALSE;
	HRESULT hr = S_OK;
	ULARGE_INTEGER liCurrPos = { 0 };

	hr = Seek(0, ESzSeek::SZ_SEEK_CUR, &liCurrPos.QuadPart);
  if (FAILED(hr))
  {
    printf("Failed to get current seek position in file '%ls'", (LPCWSTR)_szPath);
    goto LExit;
  }

	hr = Seek(newSize, ESzSeek::SZ_SEEK_SET, nullptr);
  if (FAILED(hr))
  {
    printf("Failed to set seek position in file '%ls'", (LPCWSTR)_szPath);
    goto LExit;
  }

	bRes = FALSE;
	for (unsigned i = 0; !bRes && (i < MAX_RETRIES); ++i)
	{
		bRes = ::SetEndOfFile(_hFile);
		if (!bRes)
		{
      printf("Failed to set EOF in file '%ls' on attempt %u/%u. Error 0x%08X", (LPCWSTR)_szPath, i, MAX_RETRIES, ::GetLastError());
      continue;
		}
    break;
	}
  if (!bRes)
  {
    hr = E_FAIL;
    printf("Failed to set EOF in file '%ls'", (LPCWSTR)_szPath);
    goto LExit;
  }

	hr = Seek(liCurrPos.QuadPart, ESzSeek::SZ_SEEK_SET, nullptr);
  if (FAILED(hr))
  {
    printf("Failed to set seek position in file '%ls'", (LPCWSTR)_szPath);
    goto LExit;
  }

LExit:
  return bRes ? hr : E_FAIL;
}

Z7_COM7F_IMF(CPanelSwLzmaOutStream::Write(const void* data, UInt32 size, UInt32* processedSize))
{
  BOOL bRes = TRUE;
  HRESULT hr = S_OK;
  ULARGE_INTEGER ullPos = { 0,0 };
  DWORD i = 0;

  hr = CompleteWrite();
  if (FAILED(hr))
  {
    printf("Failed to wait for previous write to complete");
    goto LExit;
  }

  if (_ullBufferSize < size)
  {
    if (_pWriteData)
    {
      delete[]_pWriteData;
    }

    _pWriteData = new unsigned char[size];
    if (!_pWriteData)
    {
      hr = E_OUTOFMEMORY;
      printf("Failed to allocate memory");
      goto LExit;
    }
    _ullBufferSize = size;
  }
  _ullWriteSize = size;
  memcpy(_pWriteData, data, size);

  _overlapped.hEvent = ::CreateEvent(nullptr, TRUE, FALSE, nullptr);
  if (!_overlapped.hEvent)
  {
    hr = HRESULT_FROM_WIN32(::GetLastError());
    printf("Failed to create event. Error 0x%08X", hr);
    goto LExit;
  }
  _overlapped.Offset = _ullNextWritePos.LowPart;
  _overlapped.OffsetHigh = _ullNextWritePos.HighPart;
  _ullNextWritePos.QuadPart += size;
  _dwWriteAttempts = 0;

  hr = WriteCore();
  if (FAILED(hr))
  {
    printf("Failed write to file '%ls'. Error 0x%08X", (LPCWSTR)_szPath, hr);
    goto LExit;
  }

LExit:
  if (processedSize)
  {
    *processedSize = FAILED(hr) ? 0 : size;
  }

  return hr;
}

HRESULT CPanelSwLzmaOutStream::CompleteWrite()
{
  HRESULT hr = S_OK;
  DWORD dwRes = ERROR_SUCCESS;

  if (_overlapped.hEvent)
  {
    for (; _dwWriteAttempts < MAX_RETRIES; ++_dwWriteAttempts)
    {
      DWORD dwWritten = 0;

      dwRes = ::WaitForSingleObject(_overlapped.hEvent, INFINITE);
      if (dwRes == WAIT_FAILED)
      {
        hr = HRESULT_FROM_WIN32(::GetLastError());
        printf("Failed to wait for write to complete on attempt %u/%u. Error 0x%08X", _dwWriteAttempts, MAX_RETRIES, hr);

        if (_dwWriteAttempts < MAX_RETRIES - 1) 
        {
          hr = WriteCore();
        }
        continue;
      }

      dwRes = ::GetOverlappedResult(_hFile, &_overlapped, &dwWritten, TRUE);
      if (!dwRes)
      {
        hr = HRESULT_FROM_WIN32(::GetLastError());
        printf("Failed to get write status on attempt %u/%u. Error 0x%08X", _dwWriteAttempts, MAX_RETRIES, hr);

        if (_dwWriteAttempts < MAX_RETRIES - 1)
        {
          hr = WriteCore();
        }
        continue;
      }

      if (dwWritten < _ullWriteSize)
      {
        printf("Failed to get write all data on attempt %u/%u, %u/%I64u written", _dwWriteAttempts, MAX_RETRIES, dwWritten, _ullWriteSize);

        if (_dwWriteAttempts < MAX_RETRIES - 1)
        {
          hr = WriteCore();
        }
        continue;
      }

      break;
    }
  }

LExit:
  if (_overlapped.hEvent)
  {
    ::CloseHandle(_overlapped.hEvent);
    memset(&_overlapped, 0, sizeof(_overlapped));
  }

  return hr;
}

HRESULT CPanelSwLzmaOutStream::WriteCore()
{
  HRESULT hr = S_OK;

  for (; _dwWriteAttempts < MAX_RETRIES; ++_dwWriteAttempts)
  {
    BOOL bRes = ::WriteFile(_hFile, _pWriteData, _ullWriteSize, nullptr, &_overlapped);
    if (!bRes && (::GetLastError() != ERROR_IO_PENDING))
    {
      hr = HRESULT_FROM_WIN32(::GetLastError());
      printf("Failed write to file '%ls' on attempt %u/%u. Error 0x%08X", (LPCWSTR)_szPath, _dwWriteAttempts, MAX_RETRIES, hr);
      continue;
    }
    break;
  }

LExit:

  return hr;
}
