/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2024 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program. If not, see http://www.gnu.org/licenses/.    **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: https://www.polyphone.io                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#ifndef OPTIONS_H
#define OPTIONS_H

#include <QStringList>
class PlayerOptions;

class Options
{
public:
    enum Mode
    {
        MODE_RESET_CONFIG = -1,
        MODE_GUI = 0,
        MODE_CONVERSION_TO_SF2 = 1,
        MODE_CONVERSION_TO_SF3 = 2,
        MODE_CONVERSION_TO_SFZ = 3,
        MODE_SYNTHESIZER = 4
    };

    Options(int argc, char *argv[]);
    ~Options();

    /// Get all files that will be open (or the file to be converted)
    QStringList getInputFiles() { return _inputFiles; }

    /// Get all files to open with possible options for the player
    QString getInputFilesAsString();

    /// Return the output file name (in case of a conversion)
    QString getOutputFile() { return _outputFile; }

    /// Return the full path of the output file
    QString getOutputFileFullPath();

    /// Return the output file (in case of a conversion)
    QString getOutputDirectory() { return _outputDirectory; }

    /// Return the mode describing how the executable will be used
    Mode mode() { return _mode; }

    /// Sfz option: preset number as prefix
    bool sfzPresetPrefix() { return _sfzPresetPrefix; }

    /// Sfz option: one directory per bank during the export
    bool sfzOneDirPerBank() { return _sfzOneDirPerBank; }

    /// Sfz option: use general midi classement
    bool sfzGeneralMidi() { return _sfzGeneralMidi; }

    /// Sf3 option: compression quality (0 is low, 1 is medium, 2 is high);
    int sf3Quality() { return _sf3Quality; }

    /// Player options
    PlayerOptions * playerOptions() { return _playerOptions; }

    /// Return true in case of bad arguments
    bool error() { return _error; }

    /// If true, display help
    bool help() { return _help; }

private:
    enum ProcessingState
    {
        STATE_INPUT_FILE,
        STATE_OUTPUT_FILE,
        STATE_OUTPUT_DIRECTORY,
        STATE_CONFIG,
        STATE_NONE
    };

    void processType1(QString arg);
    void processType2(QString arg);
    void checkErrors();
    void postTreatment();

    ProcessingState _currentState;
    QStringList _inputFiles;
    QString _outputFile, _outputDirectory;
    Mode _mode;
    bool _error, _help;

    // Sf3 option
    int _sf3Quality;

    // Sfz options
    bool _sfzPresetPrefix;
    bool _sfzOneDirPerBank;
    bool _sfzGeneralMidi;

    // Player options
    PlayerOptions * _playerOptions;

    QString _appPath;
};

#endif // OPTIONS_H
