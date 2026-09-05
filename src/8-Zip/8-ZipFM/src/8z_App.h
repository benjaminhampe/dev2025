#pragma once
#include <de/Core.h>

struct App
{
    static std::shared_ptr<App> getInstance();

    App();
    ~App();

    void init(int argc, char** argv);

    const std::string& getExeFileA() const;

    const std::string& getExeDirA() const;

    std::wstring getExeFileW() const;

    std::wstring getExeDirW() const;

private:
    int m_argc;
    char** m_argv;

    std::string m_exeFile;
    std::string m_exeDir;
};
