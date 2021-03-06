/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#ifndef GADU_SEARCH_SERVICE_H
#define GADU_SEARCH_SERVICE_H

#include <QtCore/QObject>

#include <libgadu.h>

#include "buddies/buddy.h"

#include "protocols/services/search-service.h"

class GaduProtocol;

class GaduSearchService : public SearchService
{
	Q_OBJECT

	GaduProtocol *Protocol;
	BuddySearchCriteria Query;
	unsigned int SearchSeq;
	unsigned int From;
	bool Stopped;

	friend class GaduProtocolSocketNotifiers;
	void handleEventPubdir50SearchReply(struct gg_event *e);

public:
	GaduSearchService(GaduProtocol *protocol);
	virtual void searchFirst(BuddySearchCriteria criteria);
	virtual void searchNext();
	virtual void stop();

};

#endif // GADU_SEARCH_SERVICE_H
