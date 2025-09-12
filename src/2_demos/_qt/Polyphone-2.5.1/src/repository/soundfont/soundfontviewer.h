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

#ifndef SOUNDFONTVIEWER_H
#define SOUNDFONTVIEWER_H

#include <QWidget>
class SoundfontFilter;
class UrlReader;
class UploadingDialog;

namespace Ui {
class SoundfontViewer;
}

class SoundfontViewer : public QWidget
{
    Q_OBJECT

public:
    explicit SoundfontViewer(QWidget *parent = nullptr);
    ~SoundfontViewer();

    void initialize(int soundfontId, bool forceReload);
    int getSoundfontId() { return _soundfontId; }

signals:
    void itemClicked(SoundfontFilter * filter);

private slots:
    void onDetailsReady(int soundfontId);
    void onDetailsFailed(int soundfontId, QString error);
    void on_pushRetry_clicked();
    void on_pushCancel_clicked();
    void on_pushSave_clicked();
    void on_pushEdit_clicked();
    void postProgressChanged(int progress);
    void postCompleted(QString error);
    void onUploadCancel();
    void onRefreshReady(QString error);

private:
    Ui::SoundfontViewer *ui;
    UrlReader * _urlReader;
    int _soundfontId;
    UploadingDialog * _waitingDialog;
};

#endif // SOUNDFONTVIEWER_H
