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

#include "dialogchangelog.h"
#include "ui_dialogchangelog.h"
#include <QFile>
#include <QTextStream>
#include <QDesktopServices>
#include "contextmanager.h"

DialogChangeLog::DialogChangeLog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogChangeLog)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setWindowFlags((windowFlags() & ~Qt::WindowContextHelpButtonHint));

    // Style
    QColor highlightColor = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND);
    QMap<QString, QString> replacement;
    replacement["currentColor"] = ContextManager::theme()->getColor(ThemeManager::WINDOW_TEXT).name();
    replacement["secondColor"] = highlightColor.name();
    ui->labelLogo->setPixmap(QPixmap(":/misc/polyphone.png").scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    // Version
    ui->labelTitle->setText(tr("Thank you for having installed") + "\n" +
                            tr("Polyphone") + " " + SOFT_VERSION +
                            (QString(IDENTIFIER).isEmpty() ? "" : (QString(" ") + IDENTIFIER)));
    QString versionSmall = SOFT_VERSION;
    if (versionSmall.split(".").count() > 2)
    {
        QStringList list = versionSmall.split(".");
        versionSmall = list[0] + "." + list[1];
    }

    // Read changelog and extract information
    QStringList listNew, listFix, listImprovement;
    QFile inputFile(":/misc/changelog");
    QString currentVersion = "";
    QRegularExpression rx1("^polyphone \\(([0-9]+\\.[0-9]+)\\).*");
    QRegularExpression rx2("^  \\* \\((new|fix|improvement)\\) (.*)");
    if (inputFile.open(QIODevice::ReadOnly))
    {
       QTextStream in(&inputFile);
       while (!in.atEnd())
       {
          QString line = in.readLine();

          QRegularExpressionMatch match = rx1.match(line);
          if (match.hasMatch())
              currentVersion = match.captured(1);
          else if (currentVersion == versionSmall)
          {
              match = rx2.match(line);
              if (match.hasMatch())
              {
                  if (match.captured(1) == "new")
                      listNew << match.captured(2);
                  else if (match.captured(1) == "improvement")
                      listImprovement << match.captured(2);
                  else if (match.captured(1) == "fix")
                      listFix << match.captured(2);
              }
          }
       }
       inputFile.close();
    }

    // Prepare and fill text
    QString text;
    if (!listNew.empty())
        text += "<h2><font color='" + highlightColor.name() +  "'>" + tr("What is new") + "</font></h2>" +
                " &#9679; " + listNew.join("<br/> &#9679; ");
    if (!listImprovement.empty())
        text += "<h2><font color='" + highlightColor.name() +  "'>" + tr("What has improved") + "</font></h2>" +
                " &#9679; " + listImprovement.join("<br/> &#9679; ");
    if (!listFix.empty())
        text += "<h2><font color='" + highlightColor.name() +  "'>" + tr("What is fixed") + "</font></h2>" +
                " &#9679; " + listFix.join("<br/> &#9679; ");
    ui->textChangelog->setText(text);
}

DialogChangeLog::~DialogChangeLog()
{
    delete ui;
}

void DialogChangeLog::on_pushDonate_clicked()
{
    QDesktopServices::openUrl(QUrl("https://www.polyphone.io/donate"));
}

void DialogChangeLog::on_pushOk_clicked()
{
    this->close();
}
