#pragma once
#include <QWidget>
#include <QPainter>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QFileInfo>
#include <QRandomGenerator>
#include <QMenu>
#include <QTimer>
#include <QScrollBar>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QVBoxLayout>
#include <QScrollArea>
#include <vector>

namespace de {
namespace gui {

// ============================================
struct Skin
// ============================================
{
public:
    int zoom = 100; // in percent
	
    QColor windowColor = QColor(129,129,129);
	QColor panelColor = QColor(165,165,165);
    QColor titleColor = QColor(222,222,222);
    QColor textColor = QColor(36,36,36);
	
    int shellSpacing = 1;
    int shellDropWidth = 20;	
	
    //QColor m_primary;
    //QColor m_background;
    //QFont m_font;	
};

/*
MyWidget::MyWidget(QWidget* parent)
    : QWidget(parent)
{
    connect(&SkinManager::instance(), 
			&SkinManager::skinChanged,
            this, 
			&MyWidget::onSkinChanged);

    onSkinChanged(); // initial anwenden
}

void MyWidget::onSkinChanged() {
    m_color = SkinManager::instance().primaryColor();
    m_font  = SkinManager::instance().defaultFont();
    update(); // neu zeichnen
}
*/

// ============================================
class SkinManager : public QObject 
// ============================================
{
    Q_OBJECT
public:
    static SkinManager& instance()
	{
		static SkinManager s_instance;
		return s_instance;
	}

	const Skin& current() const 
	{ 
		return m_current; 
	}
	
	// void loadSkin(const QString& name);
    
signals:
    void skinChanged();

private:
    SkinManager() = default;

	Skin m_current;
};

class SkinWidget : public QWidget
{
	Q_OBJECT
public:
	SkinWidget( QWidget* parent = nullptr )
		: QWidget( parent )
	{
		setContentMargins(0,0,0,0);
		connect(&SkinManager::instance(), &SkinManager::skinChanged, this, &SkinWidget::applySkin);
		applySkin(); // initial anwenden
	}

protected slots:
    virtual void applySkin() 
	{
        // default: nothing
		// m_color = SkinManager::instance().primaryColor();
		// m_font  = SkinManager::instance().defaultFont();
		update(); // neu zeichnen
    }
	
}

} // end namespace gui
} // end namespace de
