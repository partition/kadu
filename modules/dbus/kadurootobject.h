/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Bartłomiej Zimoń (uzi18@o2.pl)
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

#ifndef KADUROOTOBJECT_H
#define KADUROOTOBJECT_H

#include <QObject>
#include <QMap>
#include <QVariantMap>

class Account;
class Status;

class KaduRootObject : public QObject
{
	QMap<Account,Status> AccountStatus;

	Q_OBJECT
	Q_CLASSINFO("D-Bus Interface", "im.kadu")

public:
	KaduRootObject(QObject *parent = 0);
	~KaduRootObject();

public slots:
	// print version
	QString Version();
	// store status and disconnect
	void Disconnect();
	// connect and restore status
	void Reconnect();

	void Show();
	void Hide();

	// kadu quit
	void Quit();
};

#endif
