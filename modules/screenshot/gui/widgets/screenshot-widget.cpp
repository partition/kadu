/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
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

#include <QtCore/QBuffer>
#include <QtCore/QTimer>
#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>
#include <QtGui/QHBoxLayout>
#include <QtGui/QMouseEvent>

#include "configuration/configuration-file.h"
#include "debug.h"

#include "gui/widgets/crop-image-widget.h"
#include "gui/widgets/screenshot-tool-box.h"

#include "screenshot-widget.h"

ScreenshotWidget::ScreenshotWidget(QWidget *parent) :
		QWidget(parent), ButtonPressed(false), ShotMode(ShotModeStandard)
{
	setWindowRole("kadu-screenshot");

	setWindowFlags(Qt::Tool | Qt::CustomizeWindowHint
			| Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->setMargin(0);
	layout->setContentsMargins(0, 0, 0, 0);

	CropWidget = new CropImageWidget(this);
	connect(CropWidget, SIGNAL(pixmapCropped(QPixmap)), this, SLOT(pixmapCapturedSlot(QPixmap)));
	connect(CropWidget, SIGNAL(canceled()), this, SLOT(canceled()));
	layout->addWidget(CropWidget);
}

ScreenshotWidget::~ScreenshotWidget()
{
}

void ScreenshotWidget::setShotMode(ScreenShotMode shotMode)
{
	ShotMode = shotMode;
}

void ScreenshotWidget::setPixmap(QPixmap pixmap)
{
	CropWidget->setPixmap(pixmap);

	resize(pixmap.size());
}

void ScreenshotWidget::pixmapCapturedSlot(QPixmap pixmap)
{
	hide();

	emit pixmapCaptured(pixmap);
	deleteLater();
}

void ScreenshotWidget::canceled()
{
	hide();
	deleteLater();
}
