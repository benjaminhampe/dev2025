#pragma once
#include <DarkImage.h>
#include <QObject>
#include <QColor>

struct Skin
{
    int zoom = 100;
};

// ============================================
class App : public QObject
// ============================================
{
    Q_OBJECT
public:
    App(QObject* parent = nullptr);
    ~App();

    //=========================
    // SkinApi
    //=========================
    static std::shared_ptr<App> instance();
    const Skin& currentSkin() const;
    Skin& getSkin();
    int getZoom() const;

protected:
public slots:
    void setZoom(int percent);

signals:
    void skinChanged();

private:
    static std::shared_ptr<App> m_pInstance;

    Skin m_skin;
};
