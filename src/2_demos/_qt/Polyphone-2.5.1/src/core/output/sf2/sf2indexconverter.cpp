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

#include "sf2indexconverter.h"
#include "soundfontmanager.h"

Sf2IndexConverter::Sf2IndexConverter(EltID id)
{
    _indexes = SoundfontManager::getInstance()->getSiblings(id);
}

int Sf2IndexConverter::getIndexOf(int index, bool isModDestOper)
{
    int correspondingIndex = 0;

    if (isModDestOper)
    {
        if (index < 32768) // Same destination
            correspondingIndex = index;
        else if (_indexes.contains(index - 32768)) // Adapt the destination mod
            correspondingIndex = 32768 + _indexes.indexOf(index - 32768);
    }
    else if (_indexes.contains(index))
        correspondingIndex = _indexes.indexOf(index);

    return correspondingIndex;
}
