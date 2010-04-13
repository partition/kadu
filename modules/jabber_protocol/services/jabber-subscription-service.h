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

#ifndef JABBER_SUBSCRIPTION_SERVICE_H
#define JABBER_SUBSCRIPTION_SERVICE_H

#include <QtCore/QObject>

#include "iris/xmpp_jid.h"

class JabberProtocol;

class JabberSubscriptionService : public QObject
{
	Q_OBJECT

	JabberProtocol *Protocol;

private slots:
	void subscription(const XMPP::Jid &jid, const QString &type, const QString &nick);

public:
	explicit JabberSubscriptionService(JabberProtocol *protocol);

};

#endif // JABBER_SUBSCRIPTION_SERVICE_H