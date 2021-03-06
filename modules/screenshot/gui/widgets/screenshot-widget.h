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

#ifndef SCREENSHOT_WIDGET_H
#define SCREENSHOT_WIDGET_H

#include <QtGui/QPixmap>
#include <QtGui/QWidget>

#include "screenshot-mode.h"

class CropImageWidget;

class ScreenshotWidget : public QWidget
{
	Q_OBJECT

	CropImageWidget *CropWidget;

	bool ButtonPressed;
	QTimer *ToolBoxTimer;

	ScreenShotMode ShotMode;
	QRect ShotRegion;
	bool ShowPaintRect;

private slots:
	void pixmapCapturedSlot(QPixmap pixmap);
	void canceled();

public:
	explicit ScreenshotWidget(QWidget *parent = 0);
	virtual ~ScreenshotWidget();

	void setShotMode(ScreenShotMode shotMode);
	void setPixmap(QPixmap pixmap);

signals:
	void pixmapCaptured(QPixmap pixmap);
	void closed();

};

#endif // SCREENSHOT_WIDGET_H
