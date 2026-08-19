#include "PianoWidget.h"
#include <QGridLayout>
#include <QVBoxLayout>
#include <QDial>
#include <QLabel>

// ===============================================
PianoWidget::PianoWidget(QWidget* parent) 
// ===============================================
    : QWidget(parent) 
{
    QGridLayout* gridLayout = new QGridLayout(this);
    setLayout(gridLayout);

    int midiNumber = 21; // Beginn der MIDI-Nummerierung bei A0
    int row = 0;

    while (midiNumber <= 108) {
        for (int col = 0; col < 12; ++col) {
            if (midiNumber > 108) break; // Wenn die MIDI-Nummer 108 überschreitet, beenden

            QWidget* noteWidget = createNoteWidget(midiNumber);
            gridLayout->addWidget(noteWidget, row, col);
            
            if (++midiNumber % 12 == 0) break; // Wechsel zur nächsten Oktave (C-Ton gefunden)
        }
        ++row;
    }
}

PianoWidget::~PianoWidget()
{
}

QWidget* 
PianoWidget::createNoteWidget(int midiNumber) 
{
    QWidget* widget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(widget);

    QDial* frequencyDial = new QDial();
    frequencyDial->setRange(0, 100); // Beispielbereich für Frequenzanpassung
    QDial* centDial = new QDial();
    centDial->setRange(-50, 50); // Bereich für Cent-Parameter

    QLabel* label = new QLabel(QString("MIDI %1").arg(midiNumber));

    layout->addWidget(frequencyDial);
    layout->addWidget(centDial);
    layout->addWidget(label);

    widget->setLayout(layout);
    return widget;
}
