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

#ifndef LIVEEQ_H
#define LIVEEQ_H

#include "iir/Butterworth.h"

class LiveEQ
{
public:
    LiveEQ();

    // Initialize the sample rate
    void setSampleRate(unsigned int sampleRate);

    // Configuration of the equalizer
    void on();
    void off();
    void setValues(int values[10]);

    // Filter data
    void filterData(float * dataR, float * dataL, unsigned int len);

private:
    unsigned int _sampleRate;
    volatile bool _isOn;
    Iir::Butterworth::BandPass<4> _passBandsR[10];
    Iir::Butterworth::BandPass<4> _passBandsL[10];
    volatile float _coeff[10];
    int _crossFade;
    static int CROSSFADE_LENGTH;
    static float CORRECTION;
};

#endif // LIVEEQ_H
