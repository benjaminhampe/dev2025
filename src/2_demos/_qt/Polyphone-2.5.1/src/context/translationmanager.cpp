/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2024 Davy Triponney                                **
**                2014      Andrea Celani                                 **
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

#include "translationmanager.h"
#include "contextmanager.h"
#include <QLocale>
#include <QApplication>
#include <QTranslator>
#include <QDirIterator>
#include <QDebug>

const QString TranslationManager::DEFAULT_LANGUAGE = "en"; // English is the default language;
const QString TranslationManager::RESOURCE_PATH = ":/i18n";
const QString TranslationManager::TRANSLATION_DIRECTORY = "translations";

TranslationManager::TranslationManager(ConfManager * configuration) :
    _configuration(configuration),
    _translator(new QTranslator())
{
    // Native language
    _languages["en"] = QLocale("en").nativeLanguageName();

    // Languages in embedded translation file
    this->addTranslations(RESOURCE_PATH);

    // Languages in the translation directory
    this->addTranslations(QDir::currentPath() + "/" + TRANSLATION_DIRECTORY);
    this->addTranslations(_configuration->getConfigDir() + "/" + TRANSLATION_DIRECTORY);
}

TranslationManager::~TranslationManager()
{
    delete _translator;
}

void TranslationManager::addTranslations(QString path)
{
    if (QDir(path).exists())
    {
        QDirIterator it(path, QDirIterator::Subdirectories);
        while (it.hasNext())
        {
            // Language
            QString locale = it.next().section('_', 1, 1).section(".", 0, 0);
            QString languageName = QLocale(locale).nativeLanguageName();
            if (languageName != "")
                _languages[locale] = languageName;
        }
    }
}

void TranslationManager::translate()
{
    // Language used
    QString language = _configuration->getValue(
                ConfManager::SECTION_NONE, "language",
                QLocale::system().name().section('_', 0, 0)).toString();

    if (!_languages.keys().contains(language))
        language = DEFAULT_LANGUAGE;

    // First try to find an additional file
    QString fileName = "/polyphone_" + language + ".qm";
    if (QFile::exists(QDir::currentPath() + "/" + TRANSLATION_DIRECTORY + fileName))
    {
        if (!_translator->load(QDir::currentPath() + "/" + TRANSLATION_DIRECTORY + fileName))
            qWarning() << "Cannot load translation" << QDir::currentPath() + "/" + TRANSLATION_DIRECTORY + fileName;
    }
    else
    {
        if (!_translator->load(RESOURCE_PATH + fileName))
            qWarning() << "Cannot load translation" << RESOURCE_PATH + fileName;
    }
    QApplication::installTranslator(_translator);
}

QString TranslationManager::getTranslation(TranslationType translationType)
{
    QString result = "";

    switch (translationType)
    {
    case TRANSLATION_STEREO_EDITING_HELP:
        result = QObject::tr("When editing one side of a stereo sample, the other side is automatically edited the same.\nThis rule applies at the sample level and also when editing a division of an instrument:\nthe division corresponding to the other channel, if any, is edited the same.\nBoth divisions must share the same note and velocity ranges.");
        break;
    }

    return result;
}
