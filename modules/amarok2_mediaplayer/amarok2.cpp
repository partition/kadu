/*
 * %kadu copyright begin%
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Tomasz Rostański (rozteck@interia.pl)
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

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusMetaType>
#include <QtDBus/QDBusReply>
#include <QtCore/QDateTime>

#include "debug.h"
#include "exports.h"
#include "../mediaplayer/mediaplayer.h"
#include "../mediaplayer/mpris_mediaplayer.h"

MPRISMediaPlayer* amarok2;

extern "C" KADU_EXPORT int amarok2_mediaplayer_init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	amarok2 = new MPRISMediaPlayer("Amarok", "org.kde.amarok");
	bool res = mediaplayer->registerMediaPlayer(amarok2, amarok2);
	return res ? 0 : 1;
}

extern "C" KADU_EXPORT void amarok2_mediaplayer_close()
{
	mediaplayer->unregisterMediaPlayer();
	delete amarok2;
	amarok2 = NULL;
}
