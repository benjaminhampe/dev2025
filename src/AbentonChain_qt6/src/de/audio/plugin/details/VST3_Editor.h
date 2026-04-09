#pragma once

#ifdef BENNI_USE_VST3

#include <QWidget>
#include <windows.h>

class VST3_Editor : public QWidget
{
    Q_OBJECT
public:
    explicit VST3_Editor(Vst3Plugin* plugin, QWidget* parent = nullptr);
    ~VST3_Editor();

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    void createHostWindow();
    void openEditor();
    void closeEditor();

    Vst3Plugin* plugin = nullptr;
    HWND hostHwnd = nullptr;
};

#endif