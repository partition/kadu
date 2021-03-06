/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
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

#ifndef GADU_CHAT_STATE_SERVICE_H
#define GADU_CHAT_STATE_SERVICE_H

#include <libgadu.h>

#include "gadu-features.h"

#include "protocols/services/chat-state-service.h"

class Chat;
class GaduProtocol;

class GaduChatStateService : public ChatStateService
{
	Q_OBJECT

	GaduProtocol *Protocol;

	bool shouldSendEvent();

#ifdef GADU_HAVE_TYPING_NOTIFY
	friend class GaduProtocolSocketNotifiers;
	void handleEventTypingNotify(struct gg_event *e);
#endif // GADU_HAVE_TYPING_NOTIFY

public:
	GaduChatStateService(GaduProtocol *parent);

	virtual void composingStarted(const Chat &chat);
	virtual void composingStopped(const Chat &chat);

	virtual void chatWidgetClosed(const Chat &chat);
	virtual void chatWidgetActivated(const Chat &chat);
	virtual void chatWidgetDeactivated(const Chat &chat);
};

#endif // GADU_CHAT_STATE_SERVICE_H
