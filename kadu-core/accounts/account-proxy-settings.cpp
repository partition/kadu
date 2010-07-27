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

#include "account-proxy-settings.h"

AccountProxySettings::AccountProxySettings()
{
}

void AccountProxySettings::setEnabled(bool enabled)
{
	Enabled = enabled;
}

bool AccountProxySettings::isEnabled()
{
	return Enabled;
}

void AccountProxySettings::setHost(const QHostAddress &host)
{
	Host = host;

	if (!Host.isNull())
		Address = Host.toString();
}

QHostAddress AccountProxySettings::host()
{
	return Host;
}

void AccountProxySettings::setAddress(const QString &address)
{
	Address = address;
	Host.clear();
}

QString AccountProxySettings::address()
{
	return Address;
}

void AccountProxySettings::setPort(int port)
{
	Port = port;
}

int AccountProxySettings::port()
{
	return Port;
}

void AccountProxySettings::setRequiresAuthentication(bool requiresAuthentication)
{
	RequiresAuthentication = requiresAuthentication;
}

bool AccountProxySettings::requiresAuthentication()
{
	return RequiresAuthentication;
}

void AccountProxySettings::setUser(const QString &user)
{
	User = user;
}

QString AccountProxySettings::user()
{
	return User;
}

void AccountProxySettings::setPassword(const QString &password)
{
	Password = password;
}

QString AccountProxySettings::password()
{
	return Password;
}