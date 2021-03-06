/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
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

#include <QtScript/QScriptEngine>
#include <QtScript/QScriptValueIterator>

#include "scripts/sms-script-manager.h"

#include "sms-gateway-manager.h"

SmsGatewayManager *SmsGatewayManager::Instance = 0;

SmsGatewayManager * SmsGatewayManager::instance()
{
	if (!Instance)
	{
		Instance = new SmsGatewayManager();
		Instance->load();
	}

	return Instance;
}

void SmsGatewayManager::destroyInstance()
{
	delete Instance;
	Instance = 0;
}

SmsGatewayManager::SmsGatewayManager()
{
}

SmsGatewayManager::~SmsGatewayManager()
{
}

void SmsGatewayManager::load()
{
	QScriptEngine *engine = SmsScriptsManager::instance()->engine();
	qint32 length = engine->evaluate("gatewayManager.items.length").toInt32();

	for (qint32 i = 0; i < length; ++i)
	{
		QScriptValue gatewayName = engine->evaluate(QString("gatewayManager.items[%1].name()").arg(i));
		QScriptValue gatewayId = engine->evaluate(QString("gatewayManager.items[%1].id()").arg(i));
		SmsGateway gateway = qMakePair(gatewayId.toString(), gatewayName.toString());

		Items.append(gateway);
	}
}
