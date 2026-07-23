#pragma once

#pragma once
#include <functional>
#include <string>
#include <thread>
#include <vector>


namespace de {

struct SpawnResult {
    int pid;            // Windows: process ID, Linux: PID
    bool ok;            // true wenn Start erfolgreich
};

class AsyncSpawn {
public:
    using Callback = std::function<void(int exitCode)>;

    static SpawnResult run(std::string cmdLine,
                            Callback onExit = nullptr);

    static SpawnResult
    spawn(const std::string& exe,
        const std::vector<std::string>& args,
        Callback onExit)
    {
        std::string cmdLine = "\"" + exe + "\"";
        for (auto& a : args)
            cmdLine += " " + a;

        return run(cmdLine, onExit);
    }

};

} // end namespace de.

/*
#include "AsyncSpawn.hpp"

int main()
{
    AsyncCommand::run(
        "echo hello && ls -l /",
        [](int exitCode){
            printf("Command finished with exit code %d\n", exitCode);
        }
    );

    auto result = AsyncSpawn::spawn(
        "/usr/bin/ls",
        { "-l", "/" },
        [](int exitCode){
            printf("Process finished with exit code %d\n", exitCode);
        }
    );

    if (!result.ok)
        printf("Spawn failed\n");
    else
        printf("Spawned PID %d\n", result.pid);

    return 0;
}
*/
