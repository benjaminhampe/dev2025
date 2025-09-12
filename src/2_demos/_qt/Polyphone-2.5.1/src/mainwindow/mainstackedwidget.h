/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2024 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program. If not, see http://www.gnu.org/licenses/.    **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: https://www.polyphone.io                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#ifndef MAINSTACKEDWIDGET_H
#define MAINSTACKEDWIDGET_H

#include <QStackedWidget>
class MainTabBar;
class QPushButton;

class MainStackedWidget : public QStackedWidget
{
    Q_OBJECT

public:
    static const QSize TAB_ICON_SIZE;

    MainStackedWidget(QWidget *parent = nullptr);
    void setControls(QPushButton * pushHome, MainTabBar * tabBar);

    int addWidgetWithTab(QWidget * widget, QString iconName, const QString &label, bool isColored);
    void removeWidgetWithTab(QWidget * widget);
    void setWidgetLabel(QWidget * widget, const QString &label);
    void setWidgetToolTip(QWidget * widget, const QString &tip);

signals:
    void tabCloseRequested(QWidget * widget);

private slots:
    void onHomeCliked(bool clicked);
    void onWidgetClicked(QWidget * widget);
    void onCurrentChanged(int index);

private:
    void styleHomeButton(bool isEnabled);

    QPushButton * _pushHome;
    MainTabBar * _tabBar;
    QPixmap _icon;
    QPixmap _iconEnabled;
};

#endif // MAINSTACKEDWIDGET_H
