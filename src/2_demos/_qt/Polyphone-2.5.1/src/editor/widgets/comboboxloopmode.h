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

#ifndef COMBOBOXLOOPMODE_H
#define COMBOBOXLOOPMODE_H

#include <QComboBox>
#include <QListView>

class ComboBoxLoopMode : public QComboBox
{
    Q_OBJECT

public:
    explicit ComboBoxLoopMode(QWidget *parent = nullptr);
    int getIndex() { return _currentIndex; }

protected:
    void showEvent(QShowEvent * event) override;
    void hidePopup() override;

private slots:
    void onActivated(int index);

private:
    bool _ignoreFirstHide;
    int _currentIndex;
};

class ComboView : public QListView
{
    Q_OBJECT

protected:
#if QT_VERSION >= 0x060000
    void initViewItemOption(QStyleOptionViewItem *option) const override;
#else
    QStyleOptionViewItem viewOptions() const;
#endif
};

#endif // COMBOBOXLOOPMODE_H
