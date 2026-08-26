#pragma once
#include <string>
#include <vector>

#include <de/Core.h>

#include <lyra/lyra.hpp>

struct Job
{
    std::vector<std::string> filesIn; // utf8
    std::vector<std::string> filesOut; // utf8

    bool bGui = false;
    bool bInstall = false;
    bool bUninstall = false;
    bool bCompress = false;
    bool bExtract = false;
    bool bForce = false; // Don't ask for overwrite
    int iQuality = -1;

    std::string str() const
    {
        std::ostringstream o;
        if (bGui) o << "-g ";
        if (bInstall) o << "-i ";
        if (bUninstall) o << "-u ";
        if (bCompress) o << "-c ";
        if (bExtract) o << "-e ";
        if (bForce) o << "-f ";
        if (iQuality > -1) o << "-q " << iQuality << " ";
        if (filesOut.size())
        {
            o << "-o " << filesOut.size() << " ";
            for (size_t i = 0; i < filesOut.size(); ++i)
            {
                o << filesOut[i] << " ";
            }
        }

        if (filesIn.size())
        {
            o << "-a " << filesIn.size() << " ";
            for (size_t i = 0; i < filesIn.size(); ++i)
            {
                o << filesIn[i] << " ";
            }
        }

        return o.str();
    }
};

// We make parser static and give distinct job struct
// To make both functions easier testable and comparable to each other
struct ArgParser
{
    static bool parseLyra(Job* m_job, int argc, char** argv)
    {
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
            lyra::opt(m_job->iQuality,"quality")["-q"]["--quality"]
                ("Compression quality 0 to 30") |
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
    }

    static bool parseBenni(Job* m_job, int argc, char** argv)
    {
        if (!m_job)
        {
            DE_ERROR("Job is nullptr")
            return false;
        }

        enum Mode { NORMAL, READ_I_LIST, READ_O_LIST };

        Mode mode = NORMAL;

        for (int i = 1; i < argc; ++i)
        {
            std::string_view arg = argv[i];

            // If we are collecting -i arguments
            if (mode == READ_I_LIST || mode == READ_O_LIST)
            {
                // Stop when encountering another flag
                if (arg.size() > 0 && arg[0] == L'-')
                {
                    mode = NORMAL;
                    // fall through to flag handling
                }
                else
                {
                    if (mode == READ_I_LIST)
                    {
                        m_job->filesIn.emplace_back(argv[i]);
                    }
                    else
                    {
                        m_job->filesOut.emplace_back(argv[i]);
                    }
                    continue;
                }
            }

            // --- flags ---
            if (arg == "-i" || arg == "--install")
            {
                m_job->bInstall = true;
                mode = NORMAL;
                break;
            }

            if (arg == "-u" || arg == "--uninstall")
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

            if (arg == "-q" || arg == "--quality") // CompressOP quality
            {
                mode = NORMAL;
                if (i + 1 < argc)
                {
                    i++; // Advance to data argument
                    m_job->iQuality = std::stoi(argv[i]);
                    continue;
                }
                else
                {
                    DE_ERROR("Quality value missing, malformed cli.")
                    return false;
                }
            }

            // --- list of input files ---
            if (arg == "-a" || arg == "--add")
            {
                mode = READ_I_LIST;
                continue;
            }

            // --- list of output files ---
            if (arg == "-o" || arg == "--out")
            {
                mode = READ_O_LIST;
                continue;
            }

            // // --- positional files ---
            // m_job->filesIn.emplace_back(argv[i]);
        }

        return true;
    }


};
