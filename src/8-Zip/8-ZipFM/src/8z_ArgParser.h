#pragma once
#include <de/Core.h>

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
    int iPreset = -1;

    std::string baseDir;
    std::string baseName;
    std::string extension;

    std::string str() const
    {
        std::ostringstream o;
        if (baseDir.size()) o << "-b " << baseDir;
        if (baseName.size()) o << "-n " << baseName;
        if (extension.size()) o << "-x " << extension;

        if (bGui) o << "-g ";
        if (bInstall) o << "-i ";
        if (bUninstall) o << "-u ";
        if (bCompress) o << "-c ";
        if (bExtract) o << "-e ";
        if (bForce) o << "-f ";
        if (iPreset > -1) o << "-p " << iPreset << " ";
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
    static bool parseLyra(Job* m_job, int argc, char** argv);

    static bool parseBenni(Job* m_job, int argc, char** argv);
};
