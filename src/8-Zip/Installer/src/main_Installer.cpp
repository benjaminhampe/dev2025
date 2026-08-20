#include <de/win32/win32_Installer.h>
#include <DarkImage.h>

// =============================================================
int main(int argc, char** argv)
// =============================================================
{
    DE_DEBUG("argc = ",argc)
    for (int i = 0; i < argc; ++i)
    {
        DE_DEBUG("argv[",i,"] = ",argv[i])
    }

    std::string argv1;

    if (argc >= 2)
    {
        if (argv[1])
        {
            argv1 = argv[1];
        }
    }

    if (argv1 == "--install")
    {
        win32_Install();
    }
    else if (argv1 == "--uninstall")
    {
        win32_Uninstall();
    }
    else
    {
        DE_ERROR("No valid argument .exe --install|--uninstall")
    }

    return 0;
}
