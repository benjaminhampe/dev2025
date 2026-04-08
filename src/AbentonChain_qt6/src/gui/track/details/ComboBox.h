#pragma once
#include <QComboBox>

// ============================================
class ComboBox : public QComboBox
// ============================================
{
    Q_OBJECT
public:
    ComboBox(QWidget* parent = nullptr);
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    void applySkin();

protected slots:
    // void onToggled( bool checked );

protected:
    // void paintEvent(QPaintEvent* event) override;


private:
    //int m_baseWidth = 30; // -6 = symbolSize
    int m_baseHeight = 26;
    int m_height;
};
