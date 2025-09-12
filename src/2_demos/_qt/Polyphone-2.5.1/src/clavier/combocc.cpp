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

#include "combocc.h"
#include "modulatorcombosrc.h"
#include <QStandardItemModel>

ComboCC::ComboCC(QWidget *parent) : QComboBox(parent),
    _comboGroup(nullptr)
{
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentIndexChanged(int)));

    // Populate with the list of midi continuous controllers
    this->blockSignals(true);
    int count = 0;
    for (int i = 1; i < 6; i++)
    {
        this->addItem(ModulatorComboSrc::getIndexName(i, true), i);
        _controllerToIndex[i] = count++;
    }
    for (int i = 7; i < 32; i++)
    {
        this->addItem(ModulatorComboSrc::getIndexName(i, true), i);
        _controllerToIndex[i] = count++;
    }
    for (int i = 64; i < 96; i++)
    {
        this->addItem(ModulatorComboSrc::getIndexName(i, true), i);
        _controllerToIndex[i] = count++;
    }
    for (int i = 102; i < 120; i++)
    {
        this->addItem(ModulatorComboSrc::getIndexName(i, true), i);
        _controllerToIndex[i] = count++;
    }
    this->blockSignals(false);
}

void ComboCC::setGroup(QMap<ComboCC *, int> * comboGroup)
{
    _comboGroup = comboGroup;
    _comboGroup->insert(this, -1);
}

ComboCC::~ComboCC() {}

void ComboCC::selectCC(int number)
{
    if (_controllerToIndex.contains(number))
    {
        int newIndex = _controllerToIndex[number];
        this->setCurrentIndex(newIndex);
        onCurrentIndexChanged(newIndex);
    }
}

int ComboCC::getCurrentCC()
{
    return this->itemData(this->currentIndex()).toInt();
}

void ComboCC::onCurrentIndexChanged(int index)
{
    Q_UNUSED(index)

    // Update exclusions
    _comboGroup->insert(this, getCurrentCC());
    foreach (ComboCC * combo, _comboGroup->keys())
        if (combo != this)
            combo->updateEnableState();
}

void ComboCC::updateEnableState()
{
    // List of excluded controllers
    QList<int> excludedCCs;
    foreach (ComboCC * combo, _comboGroup->keys())
        if (combo != this)
            excludedCCs << _comboGroup->value(combo);

    // Set the enabled states
    QStandardItemModel * model = qobject_cast<QStandardItemModel *>(this->model());
    for (int i = 0; i < this->count(); i++)
    {
        QStandardItem * item = model->item(i);
        item->setFlags(!excludedCCs.contains(this->itemData(i).toInt()) ?
                           item->flags() | Qt::ItemIsEnabled :
                           item->flags() & ~Qt::ItemIsEnabled);
    }
}

QSize ComboCC::minimumSizeHint() const
{
    QSize size = QComboBox::minimumSizeHint();
    return QSize(150, size.height());
}
