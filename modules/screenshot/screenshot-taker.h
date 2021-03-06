/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef SCREENSHOT_TAKER_H
#define SCREENSHOT_TAKER_H

#include <QtGui/QPixmap>
#include <QtGui/QWidget>

class ChatWidget;

class ScreenshotTaker : public QWidget
{
	Q_OBJECT

	ChatWidget *CurrentChatWidget;

private slots:
	void takeShot();

protected:
	virtual void closeEvent(QCloseEvent *e);
	virtual void mousePressEvent(QMouseEvent *e);
	virtual void mouseReleaseEvent(QMouseEvent *e);

public:
	explicit ScreenshotTaker(ChatWidget *chatWidget, QWidget *parent = 0);
	virtual ~ScreenshotTaker();

public slots:
	void takeStandardShot();
	void takeShotWithChatWindowHidden();
	void takeWindowShot();

signals:
	void screenshotTaken(QPixmap screenshot, bool needsCrop);
	void screenshotNotTaken();

};

#endif // SCREENSHOT_TAKER_H
