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

#include "buddies/buddy-additional-data-delete-handler.h"

#include "buddy-additional-data-delete-handler-manager.h"

BuddyAdditionalDataDeleteHandlerManager *BuddyAdditionalDataDeleteHandlerManager::Instance = 0;

BuddyAdditionalDataDeleteHandlerManager * BuddyAdditionalDataDeleteHandlerManager::instance()
{
	if (!Instance)
		Instance = new BuddyAdditionalDataDeleteHandlerManager();

	return Instance;
}

BuddyAdditionalDataDeleteHandlerManager::BuddyAdditionalDataDeleteHandlerManager(QObject *parent) :
		QObject(parent)
{

}

BuddyAdditionalDataDeleteHandlerManager::~BuddyAdditionalDataDeleteHandlerManager()
{

}

BuddyAdditionalDataDeleteHandler * BuddyAdditionalDataDeleteHandlerManager::byName(const QString &name)
{
	foreach (BuddyAdditionalDataDeleteHandler *handler, Items)
		if (name == handler->name())
			return handler;

	return 0;
}

void BuddyAdditionalDataDeleteHandlerManager::registerAdditionalDataDeleteHandler(BuddyAdditionalDataDeleteHandler *handler)
{
	Items.append(handler);
	emit additionalDataDeleteHandlerRegistered(handler);
}

void BuddyAdditionalDataDeleteHandlerManager::unregisterAdditionalDataDeleteHandler(BuddyAdditionalDataDeleteHandler *handler)
{
	Items.removeAll(handler);
	emit additionalDataDeleteHandlerUnregistered(handler);
}
