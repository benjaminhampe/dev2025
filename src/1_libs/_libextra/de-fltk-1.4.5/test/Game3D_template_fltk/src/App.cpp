#include "App.h"

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
    m_exeFile = m_argv[0];
    // m_exeDir = dbFileDir(m_exeFile);

//<debug>
    // DE_DEBUG("argc = ",m_argc)
    for (int i = 0; i < m_argc; ++i)
    {
        // DE_DEBUG("argv[",i,"] = ", m_argv[i])
    }
//</debug>

    return true;
}

const std::string& App::getExeFileA() const
{
    return m_exeFile;
}

const std::string& App::getExeDirA() const
{
    return m_exeDir;
}

/*
std::wstring App::getExeFileW() const
{
    return de_wstr(m_exeFile);
}

std::wstring App::getExeDirW() const
{
    return de_wstr(m_exeDir);
}
*/

void add_output(const char *format, ...)
{
    va_list args;
    char line_buffer[10000];
    va_start(args, format);
    vsnprintf(line_buffer, sizeof(line_buffer)-1, format, args);
    va_end(args);

    auto& ui = App::getInstance()->ui;

    ui.logbox->log_info(line_buffer);
    // ui.logbox->buffer()->append(line_buffer);
    // ui.logbox->scroll(10000, 0);
    // ui.logbox->redraw();
}
