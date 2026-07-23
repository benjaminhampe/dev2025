#include <DarkAsyncSpawn.h>

#if defined(_WIN32)

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

namespace de {

SpawnResult AsyncSpawn::run(std::string cmdLine, Callback onExit)
{
    STARTUPINFOA si{};
    si.cb = sizeof(si);

    std::string cmd = "cmd.exe /C \"" + cmdLine + "\"";

    PROCESS_INFORMATION pi{};
    BOOL ok = CreateProcessA(
        nullptr,
        cmd.data(),
        nullptr,
        nullptr,
        FALSE,
        0,
        nullptr,
        nullptr,
        &si,
        &pi
    );

    if (!ok)
        return { -1, false };

    int pid = static_cast<int>(pi.dwProcessId);

    if (onExit) {
        std::thread([h = pi.hProcess, onExit] {
            WaitForSingleObject(h, INFINITE);

            DWORD exitCode = 0;
            GetExitCodeProcess(h, &exitCode);
            CloseHandle(h);

            onExit(static_cast<int>(exitCode));
        }).detach();
    }

    CloseHandle(pi.hThread);
    return { pid, true };
}

#if 0

SpawnResult AsyncSpawn::spawn(const std::string& path,
                              const std::vector<std::string>& args,
                              Callback onExit)
{
    std::string cmd = "\"" + path + "\"";
    for (auto& a : args)
        cmd += " " + a;

    STARTUPINFOA si{};
    si.cb = sizeof(si);

    PROCESS_INFORMATION pi{};
    BOOL ok = CreateProcessA(
        nullptr,
        cmd.data(),
        nullptr,
        nullptr,
        FALSE,
        0,
        nullptr,
        nullptr,
        &si,
        &pi
    );

    if (!ok)
        return { -1, false };

    int pid = static_cast<int>(pi.dwProcessId);

    if (onExit) {
        std::thread([h = pi.hProcess, onExit] {
            WaitForSingleObject(h, INFINITE);

            DWORD exitCode = 0;
            GetExitCodeProcess(h, &exitCode);
            CloseHandle(h);

            onExit(static_cast<int>(exitCode));
        }).detach();
    }

    CloseHandle(pi.hThread);
    return { pid, true };
}

#endif
} // end namespace de.

#endif

