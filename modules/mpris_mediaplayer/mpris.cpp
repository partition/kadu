/*
 * %kadu copyright begin%
 * Copyright 2010 Bartlomiej Zimon (uzi18@o2.pl)
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

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusMetaType>
#include <QtDBus/QDBusReply>
#include <QtCore/QDateTime>


#include "modules.h"
#include "config_file.h"
#include "debug.h"
#include "mpris.h"

MprisPlayer* mpris;

extern "C" int mpris_mediaplayer_init()
{
	mpris = new MprisPlayer("MPRIS", QString::null);

	MainConfigurationWindow::registerUiFile(
		dataPath("kadu/modules/configuration/mpris_mediaplayer.ui"), mpris);

	bool res = mediaplayer->registerMediaPlayer(mpris, mpris);
	return res ? 0 : 1;
}

extern "C" void mpris_mediaplayer_close()
{
	MainConfigurationWindow::unregisterUiFile(
		dataPath("kadu/modules/configuration/mpris_mediaplayer.ui"), mpris);

	mediaplayer->unregisterMediaPlayer();

	delete mpris;
	mpris = NULL;
}

MprisPlayer::MprisPlayer(QString name, QString service) : MPRISMediaPlayer(name, service)
{
	setService(config_file.readEntry("MediaPlayer", "MPRISService", QString::null));
}

MprisPlayer::~MprisPlayer()
{
}

void MprisPlayer::configurationUpdated()
{
	setService(config_file.readEntry("MediaPlayer", "MPRISService", QString::null));
}

void MprisPlayer::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
}
