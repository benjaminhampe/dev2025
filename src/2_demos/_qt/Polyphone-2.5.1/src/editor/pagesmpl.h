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

#ifndef PAGE_SMPL_H
#define PAGE_SMPL_H

#include <QWidget>
#include "page.h"

namespace Ui {
    class PageSmpl;
}

class PageSmpl : public Page
{
    Q_OBJECT

public:
    explicit PageSmpl(QWidget * parent = nullptr);
    ~PageSmpl() override;

    bool isSuitableFor(ElementType elementType) override
    {
        return elementType == elementSmpl;
    }

    void getPeakFrequencies(EltID id, QList<double> &frequencies, QList<double> &factors, QList<int> &keys, QList<int> &corrections);
    static EltID getRepercussionID(EltID id);

protected:
    // Update the interface
    void updateInterface(QString editingSource) override;

    // Key "space" is pressed in the tree
    void onSpacePressedInternal() override;

private slots:
    void lecture();
    void lecteurFinished(int token);
    void setStartLoop();
    void setStartLoop(int val);
    void setEndLoop();
    void setEndLoop(int val);
    void on_pushFullLength_clicked();
    void setRootKey();
    void setRootKey(int val);
    void setTune();
    void setTune(int val);
    void setType(int index);
    void setLinkedSmpl(int index);
    void setRate(int index);
    void setSinusEnabled(bool val);
    void setGainSample(int val);
    void setStereo(bool val);
    void on_pushAutoTune_clicked();
    void onCutOrdered(int start, int end);
    bool cutSample(EltID id, quint32 start, quint32 end, EltID &createdSmplId);
    QString findDuplicateName(EltID smplId);
    QString getName(QString name, int maxCharacters, int suffixNumber);
    void on_checkLectureBoucle_clicked(bool checked);

private:
    Ui::PageSmpl *ui;
    int _currentPlayingToken;

    void updatePlayButton();
    void setRateElt(EltID id, quint32 echFinal);
    void autoTune(EltID id, int &pitch, int &correction, float &score);
    void updateSinus();
    void updateLoopQuality();
};

#endif // PAGE_SMPL_H
