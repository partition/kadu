/*
 * %kadu copyright begin%
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
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

#ifndef JABBER_URL_HANDLER_H
#define JABBER_URL_HANDLER_H

#include <QtCore/QObject>
#include <QtCore/QRegExp>

#include "url-handlers/url-handler.h"

class QAction;

class JabberUrlHandler : public QObject, public UrlHandler
{
	Q_OBJECT

	QRegExp JabberRegExp;

private slots:
	void accountSelected(QAction *action);

public:
	JabberUrlHandler();

	bool isUrlValid(const QByteArray &url);

	void convertUrlsToHtml(HtmlDocument &document);

	void openUrl(const QByteArray &url, bool disableMenu);
};

#endif // JABBER_URL_HANDLER_H
