#include "pch.h"
#include "PanelSwLzmaInStream.h"
#include <stdio.h>

CPanelSwLzmaInStream::~CPanelSwLzmaInStream()
{
	ReleaseArchive();
}

HRESULT CPanelSwLzmaInStream::InitArchive(HANDLE hArchive, DWORD64 qwContainerStartPos, DWORD64 qwContainerSize)
{
	HRESULT hr = S_OK;
	LARGE_INTEGER llPos;
	LARGE_INTEGER llPosNew;
	BOOL bRes = TRUE;

	bRes = ::DuplicateHandle(::GetCurrentProcess(), hArchive, ::GetCurrentProcess(), &_hArchive, GENERIC_READ, FALSE, 0);
  if (!bRes)
  {
    printf("Failed to duplicte handle. Error 0x%08X", ::GetLastError());
    goto LExit;
  }

	llPos.QuadPart = 0;
	llPosNew.QuadPart = 0;
	bRes = ::GetFileSizeEx(_hArchive, &llPosNew);
  if (!bRes)
  {
    printf("Failed to get archive size. Error 0x%08X", ::GetLastError());
    goto LExit;
  }

	_qwContainerStartPos = qwContainerStartPos;
	_qwContainerSize = (qwContainerSize != ULLONG_MAX) ? qwContainerSize : llPosNew.QuadPart;
	_qwBundleSize = llPosNew.QuadPart;

	llPos.QuadPart = _qwContainerStartPos;
	bRes = ::SetFilePointerEx(_hArchive, llPos, nullptr, FILE_BEGIN);
  if (!bRes)
  {
    printf("Failed to seek in archive. Error 0x%08X", ::GetLastError());
    goto LExit;
  }

LExit:
	if (FAILED(hr) || !bRes)
	{
		ReleaseArchive();
	}

	return bRes ? hr : E_FAIL;
}

void CPanelSwLzmaInStream::ReleaseArchive()
{
  if (_hArchive != INVALID_HANDLE_VALUE)
  {
    ::CloseHandle(_hArchive);
    _hArchive = INVALID_HANDLE_VALUE;
  }
	_qwContainerStartPos = 0;
	_qwContainerSize = 0;
	_qwBundleSize = 0;
}

Z7_COM7F_IMF(CPanelSwLzmaInStream::Read(void* data, UInt32 size, UInt32* processedSize))
{
	HRESULT hr = S_OK;
	LARGE_INTEGER llPos;
	LARGE_INTEGER llCurrPos;
	LARGE_INTEGER llMaxRead;
	DWORD dwActualRead = 0;
	BOOL bRes = TRUE;

	llPos.QuadPart = 0;
	bRes = ::SetFilePointerEx(_hArchive, llPos, &llCurrPos, FILE_CURRENT);
  if (!bRes)
  {
    printf("Failed to set archive seek position. Error 0x%08X", ::GetLastError());
    goto LExit;
  }

	llMaxRead.QuadPart = _qwContainerStartPos + _qwContainerSize - llCurrPos.QuadPart;
	if (llMaxRead.QuadPart <= 0)
	{
		if (processedSize)
		{
			*processedSize = 0;
		}
		goto LExit;
	}

	if (llMaxRead.QuadPart > size)
	{
		llMaxRead.QuadPart = size;
	}

	bRes = ::ReadFile(_hArchive, data, llMaxRead.LowPart, &dwActualRead, nullptr);
  if (!bRes)
  {
    printf("Failed to read from archive. Error 0x%08X", ::GetLastError());
    goto LExit;
  }

	if (processedSize)
	{
		*processedSize = dwActualRead;
	}

LExit:
  return bRes ? hr : E_FAIL;
}

Z7_COM7F_IMF(CPanelSwLzmaInStream::Seek(Int64 offset, UInt32 seekOrigin, UInt64* newPosition))
{
	HRESULT hr = S_OK;
	LARGE_INTEGER llPos;
	LARGE_INTEGER llCurrPos;
	LARGE_INTEGER llNewSeek;
	BOOL bRes = TRUE;

	llPos.QuadPart = 0;
	llCurrPos.QuadPart = 0;
	bRes = ::SetFilePointerEx(_hArchive, llPos, &llCurrPos, FILE_CURRENT);
  if (!bRes)
  {
    printf("Failed to seek in archive. Error 0x%08X", ::GetLastError());
    goto LExit;
  }

	switch (seekOrigin)
	{
	case ESzSeek::SZ_SEEK_CUR:
		llNewSeek.QuadPart = llCurrPos.QuadPart + offset;
		break;
	case ESzSeek::SZ_SEEK_END:
		llNewSeek.QuadPart = _qwContainerStartPos + _qwContainerSize + offset;
		break;
	case ESzSeek::SZ_SEEK_SET:
		llNewSeek.QuadPart = _qwContainerStartPos + offset;
		break;
	default:
		hr = E_INVALIDARG;
		goto LExit;
	}

	bRes = ::SetFilePointerEx(_hArchive, llNewSeek, &llCurrPos, FILE_BEGIN);
  if (!bRes)
  {
    printf("Failed to seek in archive. Error 0x%08X", ::GetLastError());
    goto LExit;
  }

	if (newPosition)
	{
		*newPosition = llCurrPos.QuadPart - _qwContainerStartPos;
	}

LExit:
  return bRes ? hr : E_FAIL;
}

Z7_COM7F_IMF(CPanelSwLzmaInStream::GetSize(UInt64* size))
{
	if (size)
	{
		*size = _qwContainerSize;
	}
	return S_OK;
}
