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

#include "configsectionkeyboard.h"
#include "ui_configsectionkeyboard.h"
#include "contextmanager.h"
#include "editkey.h"
#include <QTextStream>
#include <QFile>

ConfigSectionKeyboard::ConfigSectionKeyboard(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfigSectionKeyboard)
{
    ui->setupUi(this);
    this->renameComboFirstC();
    connect(ContextManager::configuration(), SIGNAL(keyboardOctaveChanged()), this, SLOT(initializeFirstC()));

    // Style
    ui->labelSubTitle1->setStyleSheet("QLabel{margin: 20px 0;}");
    ui->labelSubTitle2->setStyleSheet("QLabel{margin: 20px 0;}");
    QFont font = this->font();
    font.setBold(true);
    ui->tableKeyboardMap->horizontalHeader()->setFont(font);
    ui->tableKeyboardMap->verticalHeader()->setFont(font);
    ui->tableKeyboardMap->verticalHeader()->setDefaultAlignment(Qt::AlignHCenter);
    ui->tableKeyboardMap->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableKeyboardMap->setStyleSheet(ContextManager::theme()->getTableTheme());

    ui->spinTuningFork->setSuffix(" " + tr("Hz", "unit for Herz"));
    ui->pushDefaultTuningFork->setIcon(ContextManager::theme()->getColoredSvg(":/icons/edit-undo.svg", QSize(14, 14), ThemeManager::BUTTON_TEXT));
    ui->pushDefaultTemperament->setIcon(ContextManager::theme()->getColoredSvg(":/icons/edit-undo.svg", QSize(14, 14), ThemeManager::BUTTON_TEXT));

    // Populate the table with all keys and all octaves
    for (int j = 0; j < ui->tableKeyboardMap->columnCount(); j++)
    for (int i = 0; i < ui->tableKeyboardMap->rowCount(); i++)
    ui->tableKeyboardMap->setCellWidget(i, j, new EditKey(i, static_cast<ConfManager::Key>(j)));

    // Load the temperaments
    ui->comboTemperament->blockSignals(true);
    ui->comboTemperament->addItem(tr("Equal", "Equal musical temperament"), "Equal,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0");
    QString temperamentFile = ":/misc/temperaments.csv";
    QFile file(temperamentFile);
    if (file.open(QIODevice::ReadOnly | QFile::Text))
    {
        QTextStream in(&file);
        QString line;
        while (!(line = in.readLine()).isEmpty())
            ui->comboTemperament->addItem(line.split(",")[0], line);
    }
    ui->comboTemperament->blockSignals(false);

    // Temperament relative key
    ui->comboTemperamentRelativeKey->blockSignals(true);
    for (int i = 0; i < 12; i++)
        ui->comboTemperamentRelativeKey->addItem(ContextManager::keyName()->getKeyName(i, true, false, false, true), QString::number(i));
    ui->comboTemperamentRelativeKey->blockSignals(false);
}

ConfigSectionKeyboard::~ConfigSectionKeyboard()
{
    delete ui;
}

void ConfigSectionKeyboard::initialize()
{
    // Name of the keys in the table
    for (quint32 i = 0; i <= 12; i++)
        ui->tableKeyboardMap->horizontalHeaderItem(static_cast<int>(i))->setText(
                    ContextManager::keyName()->getKeyName(i, true, false, false, true));

    // Populate the table
    int maxWidth = 50;
    for (int j = 0; j < ui->tableKeyboardMap->columnCount(); j++)
    {
        for (int i = 0; i < ui->tableKeyboardMap->rowCount(); i++)
        {
            EditKey* key = dynamic_cast<EditKey*>(ui->tableKeyboardMap->cellWidget(i, j));
            key->updateText();
            maxWidth = qMax(maxWidth, key->sizeHint().width());
        }
    }
    ui->tableKeyboardMap->horizontalHeader()->setDefaultSectionSize(maxWidth);

    // Octave configuration
    initializeFirstC();

    // Tuning fork
    ui->spinTuningFork->blockSignals(true);
    ui->spinTuningFork->setValue(ContextManager::configuration()->getValue(ConfManager::SECTION_SOUND_ENGINE, "tuning_fork", 440).toInt());
    ui->spinTuningFork->blockSignals(false);

    // Temperament
    ui->comboTemperament->blockSignals(true);
    ui->comboTemperamentRelativeKey->blockSignals(true);
    QStringList currentTemperamentConfig = ContextManager::configuration()->getValue(ConfManager::SECTION_SOUND_ENGINE, "temperament", "").toString().split(",");
    if (currentTemperamentConfig[0].isEmpty() || ui->comboTemperament->findText(currentTemperamentConfig[0]) == -1)
        ui->comboTemperament->setCurrentIndex(0); // Equal temperament
    else
        ui->comboTemperament->setCurrentText(currentTemperamentConfig[0]);
    ui->comboTemperamentRelativeKey->setCurrentIndex(currentTemperamentConfig.size() == 14 ? currentTemperamentConfig[13].toInt() : 0);
    ui->comboTemperamentRelativeKey->setEnabled(ui->comboTemperament->currentIndex() != 0);

    ui->comboTemperament->blockSignals(false);
    ui->comboTemperamentRelativeKey->blockSignals(false);
}

void ConfigSectionKeyboard::initializeFirstC()
{
    ui->comboFirstC->blockSignals(true);
    int octaveMapping = ContextManager::configuration()->getValue(ConfManager::SECTION_KEYBOARD, "octave_offset", 3).toInt();
    if (octaveMapping >= ui->comboFirstC->count())
        octaveMapping = 3;
    else if (octaveMapping < 0)
        octaveMapping = 0;
    ui->comboFirstC->setCurrentIndex(octaveMapping);
    ui->comboFirstC->blockSignals(false);
}

void ConfigSectionKeyboard::renameComboFirstC()
{
    int nbElement = ui->comboFirstC->count();
    for (int i = 0; i < nbElement; i++)
        ui->comboFirstC->setItemText(i, ContextManager::keyName()->getKeyName(static_cast<unsigned int>(12 * i)));
}

void ConfigSectionKeyboard::on_comboFirstC_currentIndexChanged(int index)
{
    ContextManager::configuration()->setValue(ConfManager::SECTION_KEYBOARD, "octave_offset", index);
}

void ConfigSectionKeyboard::on_spinTuningFork_valueChanged(int value)
{
    ContextManager::configuration()->setValue(ConfManager::SECTION_SOUND_ENGINE, "tuning_fork", value);
}

void ConfigSectionKeyboard::on_pushDefaultTuningFork_clicked()
{
    ui->spinTuningFork->setValue(440);
}

void ConfigSectionKeyboard::on_comboTemperament_currentIndexChanged(int index)
{
    ContextManager::configuration()->setValue(ConfManager::SECTION_SOUND_ENGINE, "temperament",
                                              ui->comboTemperament->itemData(index).toString() +
                                              "," + ui->comboTemperamentRelativeKey->currentData().toString());
    ui->comboTemperamentRelativeKey->setEnabled(index != 0);
}

void ConfigSectionKeyboard::on_comboTemperamentRelativeKey_currentIndexChanged(int index)
{
    ContextManager::configuration()->setValue(ConfManager::SECTION_SOUND_ENGINE, "temperament",
                                              ui->comboTemperament->currentData().toString() +
                                              "," + ui->comboTemperamentRelativeKey->itemData(index).toString());
}

void ConfigSectionKeyboard::on_pushDefaultTemperament_clicked()
{
    ui->comboTemperament->setCurrentIndex(0);
    ui->comboTemperamentRelativeKey->setCurrentIndex(0);
}
