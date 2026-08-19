#include <HideOnCloseWidget.h>

#if 0
#include <QCloseEvent>

// ===============================================================
HideOnCloseWidget::HideOnCloseWidget(QWidget *parent)
    : QWidget(parent)
    , m_bDisableHideOnClose(false)
{
    //setWindowTitle("Gradient Editor");
    //setMinimumSize(100, 100);
    setContentsMargins(0,0,0,0);
    setWindowIcon(QIcon(":/winico"));
}

void
HideOnCloseWidget::closeEvent(QCloseEvent* event)
{
    if (m_bDisableHideOnClose)
    {
        QWidget::closeEvent(event);
    }
    else
    {
        event->ignore();   // verhindert das Schließen
        this->hide();      // stattdessen nur verstecken
        emit hiddenOnClose();
    }
    emit closed();
}

void HideOnCloseWidget::enableHideOnClose()
{
    m_bDisableHideOnClose = false;
}
void HideOnCloseWidget::disableHideOnClose()
{
    m_bDisableHideOnClose = true;
}
#endif
