/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
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

#ifndef SMS_GATEWAY_MANAGER_H
#define SMS_GATEWAY_MANAGER_H

#include <QtCore/QPair>
#include <QtCore/QStringList>

typedef QPair<QString, QString> SmsGateway;

class SmsGatewayManager
{
	Q_DISABLE_COPY(SmsGatewayManager)

	static SmsGatewayManager *Instance;

	QList<SmsGateway> Items;

	SmsGatewayManager();
	~SmsGatewayManager();

	void load();

public:
	static SmsGatewayManager * instance();
	static void destroyInstance();

	const QList<SmsGateway> & items() const { return Items; }

};

#endif // SMS_GATEWAY_MANAGER_H
