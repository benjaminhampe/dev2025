#include <DarkAsyncSpawn.h>

#if defined(__linux__) || defined(__APPLE__)
#include <spawn.h>
#include <sys/wait.h>

namespace de {

extern char **environ;

SpawnResult AsyncCommand::run(const std::string& commandLine,
                              Callback onExit)
{
    const char* shell = "/bin/sh";

    char* argv[] = {
        const_cast<char*>(shell),
        const_cast<char*>("-c"),
        const_cast<char*>(commandLine.c_str()),
        nullptr
    };

    pid_t pid;
    int status = posix_spawn(&pid, shell, nullptr, nullptr, argv, environ);

    if (status != 0)
        return { -1, false };

    if (onExit) {
        std::thread([pid, onExit] {
            int exitCode = 0;
            waitpid(pid, &exitCode, 0);
            onExit(WEXITSTATUS(exitCode));
        }).detach();
    }

    return { pid, true };
}

#if 0
SpawnResult AsyncSpawn::spawn(const std::string& path,
                              const std::vector<std::string>& args,
                              Callback onExit)
{
    std::vector<char*> argv;
    argv.reserve(args.size() + 2);

    argv.push_back(const_cast<char*>(path.c_str()));
    for (auto& a : args)
        argv.push_back(const_cast<char*>(a.c_str()));
    argv.push_back(nullptr);

    pid_t pid;
    int status = posix_spawn(&pid, path.c_str(), nullptr, nullptr, argv.data(), environ);

    if (status != 0)
        return { -1, false };

    if (onExit) {
        std::thread([pid, onExit] {
            int exitCode = 0;
            waitpid(pid, &exitCode, 0);
            onExit(WEXITSTATUS(exitCode));
        }).detach();
    }

    return { pid, true };
}
#endif

} // end namespace de.

#endif
