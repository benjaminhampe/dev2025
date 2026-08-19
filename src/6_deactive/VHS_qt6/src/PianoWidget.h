#pragma once
#include <QWidget>

class PianoWidget : public QWidget 
{
public:
    PianoWidget(QWidget* parent = nullptr);
    ~PianoWidget() override;
private:
    QWidget* createNoteWidget(int midiNumber);
};
