/*
 * %kadu copyright begin%
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "debug.h"
#include "exports.h"

#include "configuration/gui/screenshot-configuration-ui-handler.h"
#include "configuration/screen-shot-configuration.h"
#include "notify/screenshot-notification.h"
#include "screenshot-actions.h"

#include "screenshot.h"

extern "C" KADU_EXPORT int screenshot_init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	kdebugf();

	ScreenshotActions::registerActions();
	ScreenShotConfiguration::createInstance();
	ScreenShotConfigurationUiHandler::registerConfigurationUi();
	ScreenshotNotification::registerNotifications();

	return 0;
}

extern "C" KADU_EXPORT void screenshot_close()
{
	kdebugf();

	ScreenshotActions::unregisterActions();
	ScreenShotConfiguration::destroyInstance();
	ScreenShotConfigurationUiHandler::unregisterConfigurationUi();
	ScreenshotNotification::unregisterNotifiactions();
}
