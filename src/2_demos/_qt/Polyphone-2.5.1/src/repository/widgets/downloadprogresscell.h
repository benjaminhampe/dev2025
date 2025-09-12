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

#ifndef DOWNLOADPROGRESSCELL_H
#define DOWNLOADPROGRESSCELL_H

#include <QWidget>

namespace Ui {
class DownloadProgressCell;
}

class DownloadProgressCell : public QWidget
{
    Q_OBJECT

public:
    explicit DownloadProgressCell(int soundfontId, QString soundfontName, QWidget *parent = nullptr);
    ~DownloadProgressCell();

    // Update data
    void progressChanged(int percent, QString finalFilename);

    // Download canceled
    void cancel();

    // Get data
    int getPercent() { return _percent; }
    QString getFilename() { return _filename; }

signals:
    void closeMenu();

protected:
    void mousePressEvent(QMouseEvent * event) override;
    void paintEvent(QPaintEvent * event) override;

private slots:
    void on_pushCancel_clicked();

private:
    Ui::DownloadProgressCell *ui;
    int _soundfontId;
    QString _soundfontName;
    int _percent;
    QString _filename;
    QPixmap _pixClose, _pixCloseHighlighted, _pixFile, _pixFileHighlighted;
};

#endif // DOWNLOADPROGRESSCELL_H
