/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef KADU_MISC_H
#define KADU_MISC_H

#include <QtGui/QApplication>

#include "coding-conversion.h"
#include "date-time.h"
#include "path-conversion.h"

class QRect;
class QWidget;

// private
extern KADUAPI long int startTime, beforeExecTime, endingTime, exitingTime;
extern KADUAPI bool measureTime;

KADUAPI QString replacedNewLine(const QString &text, const QString &newLineText);

KADUAPI QString pwHash(const QString &text);
QString translateLanguage(const QApplication *application, const QString &locale, const bool l2n);

void KADUAPI saveWindowGeometry(const QWidget *w, const QString &section, const QString &name);
void KADUAPI loadWindowGeometry(QWidget *w, const QString &section, const QString &name, int defaultX, int defaultY, int defaultWidth, int defaultHeight);

QRect stringToRect(const QString &value, const QRect *def = NULL);
QString rectToString(const QRect& rect);

QList<int> stringToIntList(const QString &in);
QString intListToString(const QList<int> &in);

QString fixFileName(const QString &path, const QString &fn);

bool caseInsensitiveLessThan(const QString &s1, const QString &s2);

#endif // MISC_H
