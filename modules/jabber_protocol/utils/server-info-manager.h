/*
 * serverinfomanager.h
 * Copyright (C) 2006  Remko Troncon
 *
 * %kadu copyright begin%
 * Copyright 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
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

#ifndef SERVER_INFO_MANAGER_H
#define SERVER_INFO_MANAGER_H

#include <QObject>
#include <QString>

#include "xmpp_tasks.h"

class ServerInfoManager : public QObject
{
	Q_OBJECT

public:
	ServerInfoManager(XMPP::Client* client);

	const QString& multicastService() const;
	bool hasPEP() const;

signals:
	void featuresChanged();

private slots:
	void disco_finished();
	void initialize();
	void deinitialize();
	void reset();

private:
	XMPP::Client* client_;
	QString multicastService_;
	bool featuresRequested_;
	bool hasPEP_;
};

#endif