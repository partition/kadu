/*
 * %kadu copyright begin%
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <libgadu.h>

#include "protocols/protocol.h"

#include "services/multilogon/gadu-multilogon-session.h"
#include "gadu-features.h"
#include "gadu-protocol.h"

#include "gadu-multilogon-service.h"

#ifdef GADU_HAVE_MULTILOGON

static bool operator == (const gg_multilogon_id_t &left, const gg_multilogon_id_t &right)
{
	for (int i = 0; i < 8; i++)
		if (left.id[i] != right.id[i])
			return false;

	return true;
}

#endif // GADU_HAVE_MULTILOGON

GaduMultilogonService::GaduMultilogonService(Account account, QObject *parent) :
		MultilogonService(parent), MyAccount(account)
{
}

GaduMultilogonService::~GaduMultilogonService()
{
}

const QList<MultilogonSession *> & GaduMultilogonService::sessions() const
{
	return Sessions;
}

void GaduMultilogonService::killSession(MultilogonSession *session)
{
	Q_UNUSED(session)

#ifdef GADU_HAVE_MULTILOGON
	GaduProtocol *gaduProtocolHandler = dynamic_cast<GaduProtocol *>(MyAccount.protocolHandler());
	if (!gaduProtocolHandler || !gaduProtocolHandler->gaduSession())
		return;

	GaduMultilogonSession *gaduSession = dynamic_cast<GaduMultilogonSession *>(session);
	if (gaduSession)
		gg_multilogon_disconnect(gaduProtocolHandler->gaduSession(), gaduSession->id());
#endif // GADU_HAVE_MULTILOGON
}

#ifdef GADU_HAVE_MULTILOGON

bool GaduMultilogonService::containsSession(const gg_multilogon_session &session)
{
	foreach (MultilogonSession *multilogonSession, Sessions)
	{
		GaduMultilogonSession *gaduSession = static_cast<GaduMultilogonSession *>(multilogonSession);
		if (session.id == gaduSession->id())
			return true;
	}

	return false;
}

bool GaduMultilogonService::containsSession(const gg_event_multilogon_info &multilogonInfo, const gg_multilogon_id_t &id)
{
	for (int i = 0; i < multilogonInfo.count; i++)
		if (multilogonInfo.sessions[i].id == id)
			return true;

	return false;
}

void GaduMultilogonService::addNewSessions(const gg_event_multilogon_info &multilogonInfo)
{
	// this does not scale above 100 connections
	// but anyone will ever have that many?
	for (int i = 0; i < multilogonInfo.count; i++)
		if (!containsSession(multilogonInfo.sessions[i]))
		{
			GaduMultilogonSession *session = new GaduMultilogonSession(MyAccount, multilogonInfo.sessions[i]);
			emit multilogonSessionAboutToBeConnected(session);
			Sessions.append(session);
			emit multilogonSessionConnected(session);
		}
}

void GaduMultilogonService::removeOldSessions(const gg_event_multilogon_info &multilogonInfo)
{
	// this does not scale above 100 connections
	// but anyone will ever have that many?
	QList<MultilogonSession *>::iterator i = Sessions.begin();

	while (i != Sessions.end())
	{
		GaduMultilogonSession *gaduSession = static_cast<GaduMultilogonSession *>(*i);

		if (!containsSession(multilogonInfo, gaduSession->id()))
		{
			emit multilogonSessionAboutToBeDisconnected(gaduSession);
			i = Sessions.erase(i);
			emit multilogonSessionDisconnected(gaduSession);
			delete gaduSession;
		}
		else
			++i;
	}
}

void GaduMultilogonService::handleEventMultilogonInfo(gg_event *e)
{
	addNewSessions(e->event.multilogon_info);
	removeOldSessions(e->event.multilogon_info);
}

void GaduMultilogonService::removeAllSessions()
{
	QList<MultilogonSession *>::iterator i = Sessions.begin();

	while (i != Sessions.end())
	{
		GaduMultilogonSession *gaduSession = static_cast<GaduMultilogonSession *>(*i);
		emit multilogonSessionAboutToBeDisconnected(gaduSession);
		i = Sessions.erase(i);
		emit multilogonSessionDisconnected(gaduSession);
		delete gaduSession;
	}
}

#endif // GADU_HAVE_MULTILOGON
