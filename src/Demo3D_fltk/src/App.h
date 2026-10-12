#pragma once
#include <de/Core.h>

#include <FL/Fl.H>
#include <FL/platform.H>
#include <FL/Fl_Window.H>

#include "LogBox.h"
#include "Canvas.h"

struct App
{
    static std::shared_ptr<App> getInstance();

    App();
    ~App();

    bool parseCommandLine(int argc, char** argv);

    const std::string& getExeFileA() const;

    const std::string& getExeDirA() const;

    std::wstring getExeFileW() const;

    std::wstring getExeDirW() const;

    std::string getTitle() const
    {
        return "Siedler3D | 2026 (c) by benjaminhampe@gmx.de";
    }

private:
    int m_argc;
    char** m_argv;

    std::string m_exeFile;
    std::string m_exeDir;

public:
    struct UI
    {
        Canvas* canvas;

        Fl_Button* btnCanvas1;
        Fl_Button* btnCanvas2;

        LogBox* logbox;

        // Fl_Text_Display* logbox; // shared between output_win() and add_output()
    };

    UI ui;
};


void add_output(const char *format, ...);
