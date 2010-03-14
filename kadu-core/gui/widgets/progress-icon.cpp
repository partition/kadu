/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QMovie>

#include "icons-manager.h"

#include "progress-icon.h"

ProgressIcon::ProgressIcon(QWidget *parent) :
		QLabel(parent), ProgressMovie(0), State(StateFinished)
{
	ProgressMovie = new QMovie(IconsManager::instance()->iconPath("kadu_icons/please-wait-small.gif"));

	setState(StateInProgress);
}

ProgressIcon::~ProgressIcon()
{
	delete ProgressMovie;
	ProgressMovie = 0;
}

void ProgressIcon::setState(ProgressIcon::ProgressState state)
{
	if (State == state)
		return;

	State = state;

	switch (State)
	{
		case StateInProgress:
			ProgressMovie->start();
			setMovie(ProgressMovie);
			break;

		case StateFinished:
			ProgressMovie->stop();
			setPixmap(IconsManager::instance()->pixmapByPath("32x32/dialog-information.png"));
			break;

		case StateFailed:
			ProgressMovie->stop();
			setPixmap(IconsManager::instance()->pixmapByPath("32x32/dialog-warning.png"));
			break;
	}
}