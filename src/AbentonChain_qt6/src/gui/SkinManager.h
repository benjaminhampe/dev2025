#pragma once
#include "gui/Skin.h"

// ============================================
class SkinManager : public QObject 
// ============================================
{
    Q_OBJECT
public:
    const Skin& 
	current() const { return m_current; }
    
	Skin& 
	current() { return m_current; }
	
	// void loadSkin(const QString& name);

signals:
    void skinChanged();

private:
    // SkinManager() = default;

	Skin m_current;
};
