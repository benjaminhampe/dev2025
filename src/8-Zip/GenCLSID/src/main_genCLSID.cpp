#include <de/win32/win32_genCLSID.h>
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

    auto id = win32_genCLSID();

    DE_OK("CLSID ",de_mbstr(id))
    return 0;
}
