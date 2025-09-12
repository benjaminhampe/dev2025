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

#ifndef SPINBOXCENTS_H
#define SPINBOXCENTS_H

#include <QSpinBox>
#include <QLineEdit>

class LineEditCents : public QLineEdit
{
    Q_OBJECT

public:
    LineEditCents(QWidget * parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event);
    void keyPressEvent(QKeyEvent *event);

private:
    QColor _textColor;
    QFont _textFont;
};

class SpinBoxCents : public QSpinBox
{
    Q_OBJECT

public:
    SpinBoxCents(QWidget * parent = nullptr);

protected:
    virtual QString textFromValue(int val) const;
};

#endif // SPINBOXCENTS_H
