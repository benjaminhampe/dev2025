#pragma once
#include <QObject>
#include <QColor>

#define g_pGlob (AbenniApp::instance())
#define g_pSkin (&AbenniApp::instance()->m_skin)


// ============================================
class AbenniSkin : public QObject
// ============================================
{
public:
    int zoom = 100; // in percent

    // TrackChain:
    int trkChnWidgetSpacing = 1;
    int trkChnDropIndicatorWidth = 20;
    QColor trkChnFillColor = QColor(50,50,50);
    QColor trkChnBodyColor = QColor(50,50,50);
};


// ============================================
class AbenniApp : public QObject
// ============================================
{
    Q_OBJECT
public:
    AbenniApp(QObject* parent = nullptr);
    ~AbenniApp();

    static AbenniApp* instance()
    {
        if (!m_pInstance)   // Only allow one instance of class to be generated.
            m_pInstance = new AbenniApp;

        return m_pInstance;
    }

protected:
public slots:
private slots:
signals:
private:
    static AbenniApp* m_pInstance;

public:
    AbenniSkin m_skin;

};
