#pragma once
#include <QWidget>


#if 1
    typedef QWidget HideOnCloseWidget;

    // class HideOnCloseWidget : public QWidget
    // {
    //     Q_OBJECT
    // public:
    //     explicit HideOnCloseWidget(QWidget *parent = nullptr) : QWidget(parent) {}
    //     bool isHideOnClose() const { return false; }
    //     void enableHideOnClose() {}
    //     void disableHideOnClose() {}
    // signals:
    //     void closed();
    //     void hiddenOnClose(); // eigenes Signal

    // protected:
    //     void closeEvent(QCloseEvent* event) override {}
    // };
#else

class HideOnCloseWidget : public QWidget
{
    Q_OBJECT
    bool m_bDisableHideOnClose;
public:
    explicit HideOnCloseWidget(QWidget *parent = nullptr);
    bool isHideOnClose() const { return !m_bDisableHideOnClose; }
    void enableHideOnClose();
    void disableHideOnClose();
signals:
    void closed();
    void hiddenOnClose(); // eigenes Signal

protected:
    void closeEvent(QCloseEvent* event) override;
};

#endif
