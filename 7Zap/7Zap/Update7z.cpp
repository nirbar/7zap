#include "pch.h"
#include "7Zap.h"
#include "lzma-sdk/CPP/Common/MyWindows.h"
#include <Psapi.h>
#include "lzma-sdk/C/CpuArch.h"
#include "lzma-sdk/CPP/Common/MyInitGuid.h"
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
#include "UpdateCallback7Zap.h"
#include "lzma-sdk/CPP/7zip/MyVersion.h"

using namespace NWindows;
using namespace NFile;
using namespace NCommandLineParser;

extern HINSTANCE g_hInstance;
HINSTANCE g_hInstance = NULL;

extern CStdOutStream * g_ErrStream;
CStdOutStream * g_ErrStream = nullptr;

extern unsigned g_NumCodecs;
extern const CCodecInfo *g_Codecs[];

extern unsigned g_NumHashers;
extern const CHasherInfo *g_Hashers[];

extern bool g_IsNT;
bool g_IsNT = true;

DECLARE_AND_SET_CLIENT_VERSION_VAR

static int WarningsCheck(HRESULT result, const CCallbackConsoleBase& callback, const CUpdateErrorInfo& errorInfo, bool showHeaders);

extern "C" HRESULT __stdcall Update7z(const FChar* archivePath, HANDLE hCancelEvent, const int numFiles, const FChar** files, const wchar_t** entryNames)
{
    HRESULT retCode = S_OK;
    CUpdateOptions uo;

    CUpdateArchiveCommand updateCommand;
    updateCommand.ActionSet = NUpdateArchive::k_ActionSet_Update;
    uo.Commands.Add(updateCommand);

    COpenCallbackConsole openCallback;
    openCallback.Init(nullptr, nullptr, nullptr, true);

    CUpdateCallback7Zap callback(hCancelEvent, numFiles, files, entryNames);

    CUpdateErrorInfo errorInfo;
    CObjectVector<COpenType> types;
    NWildcard::CCensor censor;
    for (int i = 0; i < numFiles; ++i)
    {
        const FChar* file = files[i];
        if (file && *file)
        {
            NWildcard::CCensorPathProps censorProps;
            censorProps.WildcardMatching = DoesNameContainWildcard(file);
            censorProps.Recursive = censorProps.WildcardMatching;
            censorProps.MarkMode = NWildcard::kMark_FileOrDir;

            censor.AddItem(NWildcard::k_RelatPath, true, file, censorProps);
        }
    }

    CREATE_CODECS_OBJECT;
    codecs->Load();

    retCode = UpdateArchive(codecs,
        types,
        archivePath,
        censor,
        uo,
        errorInfo, &openCallback, &callback, true);

    callback.ClosePercents2();

    if (retCode < 0)
    {
        return retCode;
    }

    retCode = WarningsCheck(retCode, callback, errorInfo,
        true // options.EnableHeaders
    );
    return retCode;
}

static int WarningsCheck(HRESULT result, const CCallbackConsoleBase& callback, const CUpdateErrorInfo& errorInfo, bool showHeaders)
{
    int exitCode = NExitCode::kSuccess;

    if (callback.ScanErrors.Paths.Size() != 0)
    {
        exitCode = NExitCode::kWarning;
    }

    if (result != S_OK || errorInfo.ThereIsError())
    {
        // we will work with (result) later
        // throw CSystemException(result);
        return HRESULT_FROM_7ZAP(NExitCode::kFatalError);
    }

    unsigned numErrors = callback.FailedFiles.Paths.Size();
    if (numErrors != 0)
    {
        exitCode = NExitCode::kWarning;
    }

    return exitCode;
}
