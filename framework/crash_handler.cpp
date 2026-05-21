#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <dbghelp.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <format>
#include <ctime>
#include "crash_handler.h"

#pragma comment(lib, "dbghelp.lib")

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static std::filesystem::path GetCrashLogPath()
{
    wchar_t tempPath[MAX_PATH] = {};
    GetTempPathW(MAX_PATH, tempPath);
    return std::filesystem::path(tempPath) / L"SickoMenu_crash.log";
}

static std::string ExceptionCodeToString(DWORD code)
{
    switch (code) {
    case EXCEPTION_ACCESS_VIOLATION:         return "ACCESS_VIOLATION";
    case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:    return "ARRAY_BOUNDS_EXCEEDED";
    case EXCEPTION_BREAKPOINT:               return "BREAKPOINT";
    case EXCEPTION_DATATYPE_MISALIGNMENT:    return "DATATYPE_MISALIGNMENT";
    case EXCEPTION_FLT_DIVIDE_BY_ZERO:       return "FLT_DIVIDE_BY_ZERO";
    case EXCEPTION_FLT_INVALID_OPERATION:    return "FLT_INVALID_OPERATION";
    case EXCEPTION_FLT_OVERFLOW:             return "FLT_OVERFLOW";
    case EXCEPTION_FLT_STACK_CHECK:          return "FLT_STACK_CHECK";
    case EXCEPTION_FLT_UNDERFLOW:            return "FLT_UNDERFLOW";
    case EXCEPTION_ILLEGAL_INSTRUCTION:      return "ILLEGAL_INSTRUCTION";
    case EXCEPTION_IN_PAGE_ERROR:            return "IN_PAGE_ERROR";
    case EXCEPTION_INT_DIVIDE_BY_ZERO:       return "INT_DIVIDE_BY_ZERO";
    case EXCEPTION_INT_OVERFLOW:             return "INT_OVERFLOW";
    case EXCEPTION_NONCONTINUABLE_EXCEPTION: return "NONCONTINUABLE_EXCEPTION";
    case EXCEPTION_PRIV_INSTRUCTION:         return "PRIV_INSTRUCTION";
    case EXCEPTION_STACK_OVERFLOW:           return "STACK_OVERFLOW";
    default:
        return std::format("UNKNOWN (0x{:08X})", code);
    }
}

static std::string ModuleNameFromAddress(DWORD addr)
{
    HMODULE hMod = nullptr;
    char buf[MAX_PATH] = "<unknown>";
    if (GetModuleHandleExA(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCSTR>(static_cast<uintptr_t>(addr)),
            &hMod) && hMod)
    {
        GetModuleFileNameA(hMod, buf, MAX_PATH);
        const char* slash = strrchr(buf, '\\');
        if (slash) memmove(buf, slash + 1, strlen(slash));
    }
    return buf;
}

// ---------------------------------------------------------------------------
// Core handler
// ---------------------------------------------------------------------------

static LONG WINAPI SickoCrashHandler(EXCEPTION_POINTERS* pExInfo)
{
    auto crashPath = GetCrashLogPath();
    std::ofstream log(crashPath, std::ios::out | std::ios::trunc);
    if (!log.is_open())
        return EXCEPTION_CONTINUE_SEARCH;

    // ---- Timestamp --------------------------------------------------------
    std::time_t now = std::time(nullptr);
    char timeBuf[64] = {};
    ctime_s(timeBuf, sizeof(timeBuf), &now);
    log << "=== SickoMenu Crash Report ===\n";
    log << "Time: " << timeBuf;

    // ---- Exception info ---------------------------------------------------
    EXCEPTION_RECORD* pRec = pExInfo->ExceptionRecord;
    CONTEXT*          pCtx = pExInfo->ContextRecord;

    log << std::format("Exception : {} (0x{:08X})\n",
        ExceptionCodeToString(pRec->ExceptionCode),
        pRec->ExceptionCode);
    log << std::format("Address   : 0x{:08X}  in  {}\n",
        reinterpret_cast<uintptr_t>(pRec->ExceptionAddress),
        ModuleNameFromAddress(static_cast<DWORD>(
            reinterpret_cast<uintptr_t>(pRec->ExceptionAddress))));

    if (pRec->ExceptionCode == EXCEPTION_ACCESS_VIOLATION &&
        pRec->NumberParameters >= 2)
    {
        const char* kind =
            pRec->ExceptionInformation[0] == 0 ? "READ" :
            pRec->ExceptionInformation[0] == 1 ? "WRITE" : "EXECUTE";
        log << std::format("AV type   : {} at 0x{:08X}\n",
            kind,
            static_cast<DWORD>(pRec->ExceptionInformation[1]));
    }

    // ---- Registers (x86 / Win32) -----------------------------------------
    log << "\n--- Registers ---\n";
    log << std::format("EAX=0x{:08X}  EBX=0x{:08X}  ECX=0x{:08X}  EDX=0x{:08X}\n",
        pCtx->Eax, pCtx->Ebx, pCtx->Ecx, pCtx->Edx);
    log << std::format("ESI=0x{:08X}  EDI=0x{:08X}  EBP=0x{:08X}  ESP=0x{:08X}\n",
        pCtx->Esi, pCtx->Edi, pCtx->Ebp, pCtx->Esp);
    log << std::format("EIP=0x{:08X}  EFLAGS=0x{:08X}\n",
        pCtx->Eip, pCtx->EFlags);

    // ---- Stack trace (x86) -----------------------------------------------
    log << "\n--- Stack Trace ---\n";
    log << "  (Line numbers only available when SickoMenu.pdb is next to SickoMenu.dll)\n\n";

    HANDLE hProcess = GetCurrentProcess();
    HANDLE hThread  = GetCurrentThread();

    SymSetOptions(SYMOPT_LOAD_LINES | SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);
    SymInitialize(hProcess, nullptr, TRUE);

    // Make a mutable copy of the context; StackWalk modifies it
    CONTEXT ctx = *pCtx;

    STACKFRAME sf = {};
    sf.AddrPC.Offset    = ctx.Eip;  sf.AddrPC.Mode    = AddrModeFlat;
    sf.AddrFrame.Offset = ctx.Ebp;  sf.AddrFrame.Mode = AddrModeFlat;
    sf.AddrStack.Offset = ctx.Esp;  sf.AddrStack.Mode = AddrModeFlat;

    // Symbol buffer (old API, compatible with all Win32 dbghelp versions)
    char symBuf[sizeof(IMAGEHLP_SYMBOL) + 512] = {};
    IMAGEHLP_SYMBOL* pSym = reinterpret_cast<IMAGEHLP_SYMBOL*>(symBuf);
    pSym->SizeOfStruct  = sizeof(IMAGEHLP_SYMBOL);
    pSym->MaxNameLength = 512;

    IMAGEHLP_LINE lineInfo = {};
    lineInfo.SizeOfStruct = sizeof(IMAGEHLP_LINE);

    for (int frame = 0; frame < 80; ++frame)
    {
        BOOL ok = StackWalk(
            IMAGE_FILE_MACHINE_I386,
            hProcess, hThread,
            &sf, &ctx,
            nullptr,
            SymFunctionTableAccess,
            SymGetModuleBase,
            nullptr);

        if (!ok || sf.AddrPC.Offset == 0)
            break;

        DWORD pc = sf.AddrPC.Offset;

        // Module name
        std::string modName = ModuleNameFromAddress(pc);

        // Symbol name
        DWORD symDisp = 0;
        std::string symName = "??";
        if (SymGetSymFromAddr(hProcess, pc, &symDisp, pSym))
        {
            char undecorated[512] = {};
            UnDecorateSymbolName(pSym->Name, undecorated, sizeof(undecorated),
                UNDNAME_COMPLETE | UNDNAME_NO_THROW_SIGNATURES);
            symName = (undecorated[0] != '\0') ? undecorated : pSym->Name;
        }

        // File + line (only works when PDB is present)
        DWORD lineDisp = 0;
        std::string fileInfo;
        if (SymGetLineFromAddr(hProcess, pc, &lineDisp, &lineInfo))
        {
            std::string fullPath = lineInfo.FileName ? lineInfo.FileName : "";
            auto sep = fullPath.find_last_of("\\/");
            std::string fileName = (sep != std::string::npos)
                ? fullPath.substr(sep + 1)
                : fullPath;
            fileInfo = std::format("  [{} : line {}]", fileName, lineInfo.LineNumber);
        }

        log << std::format("#{:02d}  0x{:08X}  {}!{}+0x{:X}{}\n",
            frame, pc, modName, symName, symDisp, fileInfo);
    }

    SymCleanup(hProcess);

    log << "\n=== End of Crash Report ===\n";
    log.flush();
    log.close();

    // Notify the user
    std::wstring msg =
        L"SickoMenu crashed!\n\n"
        L"A crash log has been saved to:\n" +
        crashPath.wstring() +
        L"\n\nPlease share this file when reporting the crash.";
    MessageBoxW(nullptr, msg.c_str(), L"SickoMenu Crash", MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);

    return EXCEPTION_CONTINUE_SEARCH;
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

void InstallSickoCrashHandler()
{
    SetUnhandledExceptionFilter(SickoCrashHandler);
}
