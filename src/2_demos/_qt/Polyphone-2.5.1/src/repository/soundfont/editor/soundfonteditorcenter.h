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

#ifndef SOUNDFONTEDITORCENTER_H
#define SOUNDFONTEDITORCENTER_H

#include <QWidget>
class SoundfontDetails;

namespace Ui {
class SoundfontEditorCenter;
}

class SoundfontEditorCenter : public QWidget
{
    Q_OBJECT

public:
    explicit SoundfontEditorCenter(QWidget *parent = nullptr);
    ~SoundfontEditorCenter();

    // Fill the interface with the details
    void initialize(SoundfontDetails *details);

    // Possibly return an error if the editing is not valid
    QString getEditingError();

    // Get a description of the editing
    void fillArguments(QMap<QString, QString> &arguments);

    // Get the file list to upload
    QMap<QString, QString> getFileArguments();

private slots:
    void on_pushAddFile_clicked();

private:
    Ui::SoundfontEditorCenter *ui;
};

#endif // SOUNDFONTEDITORCENTER_H
