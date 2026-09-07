#include "8z_App.h"

//static
std::shared_ptr<App>
App::getInstance()
{
    static std::shared_ptr<App> s_instance = std::make_shared<App>();
    return s_instance;
}

App::App()
    : m_argc{ 0 }
    , m_argv{ nullptr }
{
}

App::~App()
{

}

bool App::parseCommandLine(int argc, char** argv)
{
    m_argc = argc;
    m_argv = argv;
    if (m_argc > 0)
    {
        m_exeFile = m_argv[0];
        m_exeDir = dbFileDir(m_exeFile);
    }
    else
    {
        DE_ERROR("Too few argc = ", argc)
    }

//<debug>
    DE_DEBUG("argc = ",m_argc)
    for (int i = 0; i < m_argc; ++i)
    {
        DE_DEBUG("argv[",i,"] = ", m_argv[i])
    }
//</debug>

    // Job jobLyra;
    // bool okLyra = ArgParser::parseLyra(&jobLyra,argc,argv);
    // DE_OK("[Lyra ] ok(",okLyra,"), job(", jobLyra.str(),")")

    bool ok = ArgParser::parseBenni(&m_job,argc,argv);
    if (ok)
    {
        DE_OK("Parser OK.")
        DE_OK("Job: ")
        DE_OK(m_job.str())
    }
    else
    {
        DE_ERROR("Parser Error.")
        DE_ERROR("Job: ")
        DE_ERROR(m_job.str())
    }
    return ok;
}

const std::string& App::getExeFileA() const
{
    return m_exeFile;
}

const std::string& App::getExeDirA() const
{
    return m_exeDir;
}

std::wstring App::getExeFileW() const
{
    return de_wstr(m_exeFile);
}

std::wstring App::getExeDirW() const
{
    return de_wstr(m_exeDir);
}

