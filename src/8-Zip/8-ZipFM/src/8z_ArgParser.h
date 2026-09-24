#pragma once
#include <de/Core.h>
#include <de/FileInfo.h>

struct Job
{
    de::FileInfos filesIn; // utf8
    // StringListA filesIn; // utf8
    // StringListA filesOut; // utf8

    bool bUpdate = false;
    bool bRestartExplorer = false;
    bool bAdmin = false;
    bool bGui = false;
    bool bTarInspector = false;
    bool bInstall = false;
    bool bUninstall = false;
    bool bCompress = false;
    bool bExtract = false;
    bool bForce = false; // Don't ask for overwrite
    int iPreset = 0;

    std::string directory; // No trailing slash allowed
    std::string fileName; // e.g. archive.zst = <baseName>.<extension>

    std::string uri() const
    {
        return directory + "/" + fileName;
    }

    std::string str(bool bPrintFileNames = false) const
    {
        std::ostringstream o;
        if (bUpdate) o << "--update ";
        if (bRestartExplorer) o << "-k ";
        if (bAdmin) o << "--admin ";
        if (fileName.size()) o << "-o " << fileName;
        if (directory.size()) o << "-d " << directory;

        if (bGui) o << "-g ";
        if (bInstall) o << "--install ";
        if (bUninstall) o << "-u ";
        if (bCompress) o << "-c ";
        if (bExtract) o << "-e ";
        if (bForce) o << "-f ";
        if (iPreset > -1) o << "-p " << iPreset << " ";
        // if (filesOut.size())
        // {
        //     o << "-o " << filesOut.size() << " ";
        //     if (bPrintFileNames)
        //     {
        //         for (size_t i = 0; i < filesOut.size(); ++i)
        //         {
        //             o << filesOut[i] << " ";
        //         }
        //     }
        // }

        if (filesIn.size())
        {
            o << "-i " << filesIn.size() << " ";
            if (bPrintFileNames)
            {
                for (size_t i = 0; i < filesIn.size(); ++i)
                {
                    o << filesIn[i].str() << "\n";
                }
            }
        }

        return o.str();
    }
};

// We make parser static and give distinct job struct
// To make both functions easier testable and comparable to each other
struct ArgParser
{
    static bool parseLyra(Job* m_job, int argc, char** argv);

    static bool parseBenni(Job* m_job, int argc, char** argv);
};
