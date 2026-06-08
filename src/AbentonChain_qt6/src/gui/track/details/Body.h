#pragma once
#include <QWidget>
#include "gui/track/details/ComboBox.h"
#include "gui/track/details/Pad.h"
//#include "gui/track/details/LoadPluginButton.h"
//#include "gui/track/details/SavePresetButton.h"
#include "gui/track/details/EnableButton.h"
#include "gui/track/details/WrenchButton.h"

// ============================================
class Body : public QWidget
// ============================================
{
    Q_OBJECT
public:
    Body(QWidget* parent = nullptr);
    //QSize sizeHint() const override;
    //QSize minimumSizeHint() const override;

    Pad* getPad() { return m_pad; }
    ComboBox* getComboPreset() { return m_cbxPreset; }
    ComboBox* getComboParam1() { return m_cbxParam1; }
    ComboBox* getComboParam2() { return m_cbxParam2; }
    void applySkin();

protected slots:

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    int m_baseWidth = 300;
    int m_baseHeight = 338;
    int m_baseMargin = 10;

    int m_width;
    int m_height;

    EnableButton* m_btnLoadPlugin;
    EnableButton* m_btnSavePreset;
    ComboBox* m_cbxPreset;
    ComboBox* m_cbxParam1;
    ComboBox* m_cbxParam2;
    Pad* m_pad;
};
