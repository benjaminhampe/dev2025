#include "8z_ArgParser.h"
//#include <lyra/lyra.hpp>
#include <de/win32/win32_Get_Explorer_Selection.h>
#include <de/FileInfo.h>

// static
bool ArgParser::parseBenni(Job* m_job, int argc, char** argv)
{
    if (!m_job)
    {
        DE_ERROR("Job is nullptr")
        return false;
    }

    StringListW filesOut;
    StringListW filesIn;

    StringListW explorerSelection = win32_Get_Explorer_Selection();
//<debug>
    /*
    DE_BENNI("explorerSelection = ",explorerSelection.size())
    for (size_t i = 0; i < explorerSelection.size(); ++i)
    {
        DE_DEBUG("explorerSelection[",i,"] ",de_mbstr(explorerSelection[i]))
    }
    */
//</debug>
    platform_addUniqueFileNames(explorerSelection,filesIn);

    enum Mode { NORMAL, READ_I_LIST, READ_O_LIST };

    Mode mode = NORMAL;

    for (int i = 1; i < argc; ++i)
    {
        std::string_view arg = argv[i];

        // If we are collecting -i arguments
        if (mode == READ_I_LIST || mode == READ_O_LIST)
        {
            // Stop when encountering another flag
            if (arg.size() > 0 && arg[0] == '-')
            {
                mode = NORMAL;
                // fall through to flag handling
            }
            else
            {
                std::wstring ws = de_wstr(argv[i]);

                if (mode == READ_I_LIST)
                {
                    platform_addUniqueFileName(ws, filesIn);
                }
                else
                {
                    platform_addUniqueFileName(ws, filesOut);
                }
                continue;
            }
        }

        // --- flags ---
        if (arg == "--update")
        {
            m_job->bUpdate = true;
            mode = NORMAL;
            break;
        }

        if (arg == "-k" || arg == "--killexplorer")
        {
            m_job->bRestartExplorer = true;
            mode = NORMAL;
            break;
        }

        if (arg == "--admin")
        {
            m_job->bAdmin = true;
            mode = NORMAL;
            break;
        }

        if (arg == "--install")
        {
            m_job->bInstall = true;
            mode = NORMAL;
            break;
        }

        if (arg == "--uninstall" || arg == "--deinstall")
        {
            m_job->bUninstall = true;
            mode = NORMAL;
            break;
        }

        if (arg == "-c" || arg == "--compress")
        {
            m_job->bCompress = true;
            mode = NORMAL;
            break;
        }

        if (arg == "-e" || arg == "--extract")
        {
            m_job->bExtract = true;
            mode = NORMAL;
            break;
        }

        if (arg == "-g" || arg == "--gui")
        {
            m_job->bGui = true;
            mode = NORMAL;
            continue;
        }

        if (arg == "-f" || arg == "--force") // ExtractOP asks to overwrite existing dir
        {
            m_job->bForce = true;
            mode = NORMAL;
            continue;
        }

        if (arg == "-p" || arg == "--preset") // Preset quality + heuristic + other settings
        {
            mode = NORMAL;
            if (i + 1 < argc)
            {
                i++; // Advance to data argument
                m_job->iPreset = std::stoi(argv[i]);
                continue;
            }
            else
            {
                DE_ERROR("Preset value missing, malformed cli.")
                return false;
            }
        }
/*
        if (arg == "-q" || arg == "--quality") // CompressOP quality
        {
            mode = NORMAL;
            if (i + 1 < argc)
            {
                i++; // Advance to data argument
                m_job->iPreset = std::stoi(argv[i]);
                continue;
            }
            else
            {
                DE_ERROR("Quality value missing, malformed cli.")
                return false;
            }
        }
*/
        // --- list of input files ---
        if (arg == "-i" || arg == "--in" || arg == "--input" || arg == "--inputs")
        {
            mode = READ_I_LIST;
            continue;
        }

        // --- list of output files ---
        if (arg == "-o" || arg == "--out" || arg == "--output" || arg == "--outputs")
        {
            mode = READ_O_LIST;
            continue;
        }

        // --- positional files ---
        platform_addUniqueFileName(de_wstr(argv[i]), filesIn);
    }


    DE_BENNI("filesOut = ",filesOut.size())
    for (size_t i = 0; i < filesOut.size(); ++i)
    {
        DE_DEBUG("filesOut[",i,"] ",de_mbstr(filesOut[i]))
    }

    DE_BENNI("filesIn = ",filesIn.size())
    for (size_t i = 0; i < filesIn.size(); ++i)
    {
        DE_DEBUG("filesIn[",i,"] ",de_mbstr(filesIn[i]))
    }

    m_job->filesOut = de_mbstr(filesOut);
    m_job->filesIn = de_mbstr(filesIn);

    return true;
}



// We make parser static and give distinct job struct
// To make both functions easier testable and comparable to each other
// static
bool ArgParser::parseLyra(Job* m_job, int argc, char** argv)
{
#if 0
    if (!m_job)
    {
        DE_ERROR("Job is nullptr")
        return false;
    }

    lyra::cli m_cli = lyra::cli() |
        lyra::opt(m_job->bGui,"gui")["-g"]["--gui"]
            ("Enable GUI") |
        lyra::opt(m_job->bInstall,"install")["-i"]["--install"]
            ("Install 8zip ShellExtension.dll") |
        lyra::opt(m_job->bUninstall,"uninstall")["-u"]["--install"]
            ("Uninstall 8zip ShellExtension.dll") |
        lyra::opt(m_job->bCompress,"compress")["-c"]["--compress"]
            ("Build compressed archive out of files and folders") |
        lyra::opt(m_job->iPreset,"preset")["-p"]["--preset"]
            ("Compression quality 1 to 36") |
        // lyra::opt(m_job->iQuality,"quality")["-q"]["--quality"]
        //     ("Compression quality 0 to 30") |
        lyra::opt(m_job->bExtract,"extract")["-e"]["--extract"]
            ("Extract archive to new folders, ask for overwrite") |
        lyra::opt(m_job->bForce,"force")["-f"]["--force"]
            ("Extract does not ask for overwrite") |
        lyra::opt(m_job->filesOut,"outputFiles")["-o"]["--out"]
            ("Space separated list of output files") |
        lyra::opt(m_job->filesIn,"inputFiles")["-a"]["--add"]
            ("Space separated list of input files and folders") |
        lyra::arg(m_job->filesIn,"inputFiles")
            ("Space separated list of input files and folders");

    auto result = m_cli.parse( { argc, argv } );
    if ( !result )
    {
        DE_ERROR("Error in command line: ", result.message())
        return false;
    }
    return true;
#else
    return false;
#endif
}
