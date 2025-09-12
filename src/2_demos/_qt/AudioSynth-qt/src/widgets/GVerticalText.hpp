#pragma once
#include "QImageWidget.hpp"
#include <QPushButton>
#include <QFont5x8.hpp>

// ============================================================================
class GVerticalText : public QImageWidget
// ============================================================================
{
   Q_OBJECT
public:
   GVerticalText( QString name, QWidget* parent = 0 );
   ~GVerticalText();

signals:
   //void toggled(bool);
public slots:

public:
   QPushButton* m_bypass;
   QFont5x8 m_font;
};
