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

#include "tooltranspose.h"
#include "tooltranspose_gui.h"
#include "tooltranspose_parameters.h"
#include "soundfontmanager.h"

ToolTranspose::ToolTranspose() : AbstractToolIterating(elementInst, new ToolTranspose_parameters(), new ToolTranspose_gui())
{

}

void ToolTranspose::beforeProcess(IdList ids)
{
    Q_UNUSED(ids)
    _elementsInError.clear();
}

void ToolTranspose::process(SoundfontManager * sm, EltID id, AbstractToolParameters *parameters)
{
    ToolTranspose_parameters * params = static_cast<ToolTranspose_parameters *>(parameters);
    
    // Elements to transpose
    EltID divId = EltID(elementInstSmpl, id.indexSf2, id.indexElt);
    if (sm->getSiblings(divId).empty())
    {
        // No divisions => error
        _mutex.lock();
        _elementsInError << sm->getQstr(id, champ_name);
        _mutex.unlock();
    }
    else
    {
        double shift = params->getSemiTones();

        // Semi tones number
        int semiTones = qRound(shift);

        // Correction
        int correction = qRound(100. * (shift - semiTones));

        // Process all linked samples
        foreach (int i, sm->getSiblings(divId))
        {
            divId.indexElt2 = i;
            AttributeValue valeur;

            // Update keyrange?
            if (params->getAdaptKeyRanges())
            {
                int noteInf = sm->get(divId, champ_keyRange).rValue.byLo;
                int noteSup = sm->get(divId, champ_keyRange).rValue.byHi;

                // Move the range
                noteInf -= semiTones;
                noteSup -= semiTones;

                // Ajustement
                if (noteInf < 0)
                    noteInf = 0;
                else if (noteInf > 127)
                    noteInf = 127;
                if (noteSup < 0)
                    noteSup = 0;
                else if (noteSup > 127)
                    noteSup = 127;

                // Save the new range
                valeur.rValue.byLo = static_cast<quint8>(noteInf);
                valeur.rValue.byHi = static_cast<quint8>(noteSup);
                sm->set(divId, champ_keyRange, valeur);
            }

            // Note de base
            EltID idSmpl = divId;
            idSmpl.typeElement = elementSmpl;
            idSmpl.indexElt = sm->get(divId, champ_sampleID).wValue;
            int rootKey = sm->get(idSmpl, champ_byOriginalPitch).bValue;
            if (sm->isSet(id, champ_overridingRootKey))
                rootKey = sm->get(id, champ_overridingRootKey).wValue;
            if (sm->isSet(divId, champ_overridingRootKey))
                rootKey = sm->get(divId, champ_overridingRootKey).wValue;

            // Modification rootkey et enregistrement
            rootKey -= semiTones;
            if (rootKey < 0)
                rootKey = 0;
            else if (rootKey > 127)
                rootKey = 127;
            valeur.wValue = static_cast<quint8>(rootKey);
            sm->set(divId, champ_overridingRootKey, valeur);

            // Correction
            int fineTune = 0;
            if (sm->isSet(id, champ_fineTune))
                fineTune = sm->get(id, champ_fineTune).shValue;
            if (sm->isSet(divId, champ_fineTune))
                fineTune = sm->get(divId, champ_fineTune).shValue;
            int coarseTune = 0;
            if (sm->isSet(id, champ_coarseTune))
                coarseTune = sm->get(id, champ_coarseTune).shValue;
            if (sm->isSet(divId, champ_coarseTune))
                coarseTune = sm->get(divId, champ_coarseTune).shValue;

            // Modification de la correction
            fineTune += correction;
            if (fineTune >= 100)
            {
                fineTune -= 100;
                coarseTune += 1;
            }
            else if (fineTune <= -100)
            {
                fineTune += 100;
                coarseTune -= 1;
            }

            // Enregistrement de la nouvelle correction
            valeur.shValue = static_cast<qint16>(fineTune);
            sm->set(divId, champ_fineTune, valeur);
            valeur.shValue = static_cast<qint16>(coarseTune);
            sm->set(divId, champ_coarseTune, valeur);
        }

        // Simplification
        sm->simplify(divId, champ_fineTune);
        sm->simplify(divId, champ_coarseTune);
    }
}

QString ToolTranspose::getWarning()
{
    QString txt = "";

    if (!_elementsInError.empty())
    {
        txt = tr("An instrument comprising no samples is not compatible with this tool:");
        txt += "<ul>";
        foreach (QString element, _elementsInError)
            txt += "<li>" + element + "</li>";
        txt += "</ul>";
    }

    return txt;
}
