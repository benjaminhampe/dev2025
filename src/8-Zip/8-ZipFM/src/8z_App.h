#pragma once
#include "8z_ArgParser.h"

struct App
{
    static std::shared_ptr<App> getInstance();

    App();
    ~App();

    bool parseCommandLine(int argc, char** argv);

    Job& getJob() { return m_job; }

    const Job& getJob() const { return m_job; }

    const std::string& getExeFileA() const;

    const std::string& getExeDirA() const;

    std::wstring getExeFileW() const;

    std::wstring getExeDirW() const;

private:
    int m_argc;
    char** m_argv;

    std::string m_exeFile;
    std::string m_exeDir;

    Job m_job;
};
