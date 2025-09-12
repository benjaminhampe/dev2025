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

#include "configsectionsound.h"
#include "ui_configsectionsound.h"
#include "contextmanager.h"

ConfigSectionSound::ConfigSectionSound(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfigSectionSound)
{
    ui->setupUi(this);

    // Style
    ui->labelSubTitle1->setStyleSheet("QLabel{margin: 20px 0;}");
    ui->labelSubTitle2->setStyleSheet("QLabel{margin: 20px 0;}");
    ui->labelSubTitle3->setStyleSheet("QLabel{margin: 20px 0;}");
}

ConfigSectionSound::~ConfigSectionSound()
{
    delete ui;
}

void ConfigSectionSound::initialize()
{
    // Volume
    int gain = ContextManager::configuration()->getValue(ConfManager::SECTION_SOUND_ENGINE, "gain", 0).toInt();
    ui->sliderGain->blockSignals(true);
    ui->sliderGain->setValue(gain);
    ui->sliderGain->blockSignals(false);
    ui->labelGain->setText((gain >= 0 ? "+" : "") + QString::number(gain) + " " + tr("dB", "unit for decibels"));

    // Reverb
    ui->dialRevNiveau->blockSignals(true);
    ui->dialRevNiveau->setValue(ContextManager::configuration()->getValue(ConfManager::SECTION_SOUND_ENGINE, "rev_level", 0).toInt());
    ui->dialRevNiveau->blockSignals(false);

    ui->dialRevProfondeur->blockSignals(true);
    ui->dialRevProfondeur->setValue(ContextManager::configuration()->getValue(ConfManager::SECTION_SOUND_ENGINE, "rev_size", 0).toInt());
    ui->dialRevProfondeur->blockSignals(false);

    ui->dialRevDensite->blockSignals(true);
    ui->dialRevDensite->setValue(ContextManager::configuration()->getValue(ConfManager::SECTION_SOUND_ENGINE, "rev_width", 0).toInt());
    ui->dialRevDensite->blockSignals(false);

    ui->dialRevAttenuation->blockSignals(true);
    ui->dialRevAttenuation->setValue(ContextManager::configuration()->getValue(ConfManager::SECTION_SOUND_ENGINE, "rev_damping", 0).toInt());
    ui->dialRevAttenuation->blockSignals(false);

    // Chorus
    ui->dialChoNiveau->blockSignals(true);
    ui->dialChoNiveau->setValue(ContextManager::configuration()->getValue(ConfManager::SECTION_SOUND_ENGINE, "cho_level", 0).toInt());
    ui->dialChoNiveau->blockSignals(false);

    ui->dialChoAmplitude->blockSignals(true);
    ui->dialChoAmplitude->setValue(ContextManager::configuration()->getValue(ConfManager::SECTION_SOUND_ENGINE, "cho_depth", 0).toInt());
    ui->dialChoAmplitude->blockSignals(false);

    ui->dialChoFrequence->blockSignals(true);
    ui->dialChoFrequence->setValue(ContextManager::configuration()->getValue(ConfManager::SECTION_SOUND_ENGINE, "cho_frequency", 0).toInt());
    ui->dialChoFrequence->blockSignals(false);

    // Other
    ui->comboVelToFilter->blockSignals(true);
    ui->comboVelToFilter->setCurrentIndex(ContextManager::configuration()->getValue(ConfManager::SECTION_SOUND_ENGINE, "modulator_vel_to_filter", 1).toInt());
    ui->comboVelToFilter->blockSignals(false);
}

void ConfigSectionSound::on_dialRevNiveau_valueChanged(int value)
{
    ContextManager::configuration()->setValue(ConfManager::SECTION_SOUND_ENGINE, "rev_level", value);
}

void ConfigSectionSound::on_dialRevProfondeur_valueChanged(int value)
{
    ContextManager::configuration()->setValue(ConfManager::SECTION_SOUND_ENGINE, "rev_size", value);
}

void ConfigSectionSound::on_dialRevDensite_valueChanged(int value)
{
    ContextManager::configuration()->setValue(ConfManager::SECTION_SOUND_ENGINE, "rev_width", value);
}

void ConfigSectionSound::on_dialRevAttenuation_valueChanged(int value)
{
    ContextManager::configuration()->setValue(ConfManager::SECTION_SOUND_ENGINE, "rev_damping", value);
}

void ConfigSectionSound::on_dialChoNiveau_valueChanged(int value)
{
    ContextManager::configuration()->setValue(ConfManager::SECTION_SOUND_ENGINE, "cho_level", value);
}

void ConfigSectionSound::on_dialChoAmplitude_valueChanged(int value)
{
    ContextManager::configuration()->setValue(ConfManager::SECTION_SOUND_ENGINE, "cho_depth", value);
}

void ConfigSectionSound::on_dialChoFrequence_valueChanged(int value)
{
    ContextManager::configuration()->setValue(ConfManager::SECTION_SOUND_ENGINE, "cho_frequency", value);
}

void ConfigSectionSound::on_sliderGain_valueChanged(int value)
{
    ContextManager::configuration()->setValue(ConfManager::SECTION_SOUND_ENGINE, "gain", value);
    ui->labelGain->setText((value > 0 ? "+" : "") + QString::number(value) + " " + tr("dB", "unit for decibels"));
}

void ConfigSectionSound::on_comboVelToFilter_currentIndexChanged(int index)
{
    ContextManager::configuration()->setValue(ConfManager::SECTION_SOUND_ENGINE, "modulator_vel_to_filter", index);
}
