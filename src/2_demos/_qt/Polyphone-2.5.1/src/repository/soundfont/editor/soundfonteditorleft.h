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

#ifndef SOUNDFONTEDITORLEFT_H
#define SOUNDFONTEDITORLEFT_H

#include <QWidget>
class SoundfontInformation;

namespace Ui {
class SoundfontEditorLeft;
}

class SoundfontEditorLeft : public QWidget
{
    Q_OBJECT

public:
    explicit SoundfontEditorLeft(QWidget *parent = nullptr);
    ~SoundfontEditorLeft();

    // Initialize the interface with the soundfont information
    void initialize(SoundfontInformation * soundfontInfo);

    // Possibly return an error if the editing is not valid
    QString getEditingError();

    // Get a description of the editing
    void fillArguments(QMap<QString, QString> &arguments);

private:
    Ui::SoundfontEditorLeft *ui;
};

#endif // SOUNDFONTEDITORLEFT_H
