/*
 * Copyright 2009 Jacek Jabłoński
 * %kadu copyright begin%
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

#include <QtCore/QMetaMethod>
#include <QtDBus/QDBusInterface>

#include "configuration/configuration-file.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "misc/path-conversion.h"
#include "notify/notification-manager.h"
#include "debug.h"

#include "kde_notify.h"
#include <QDBusReply>
#include <QTimer>
#include <notify/notification.h>

extern "C" KADU_EXPORT int kde_notify_init(bool firstLoad)
{
	kdebugf();

	kde_notify = new KdeNotify();

	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/kde_notify.ui"));
	MainConfigurationWindow::registerUiHandler(kde_notify);

	kdebugf2();
	return 0;
}

extern "C" KADU_EXPORT void kde_notify_close()
{
	kdebugf();

	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/kde_notify.ui"));
	MainConfigurationWindow::unregisterUiHandler(kde_notify);
	delete kde_notify;
	kde_notify = 0;

	kdebugf2();
}

KdeNotify::KdeNotify(QObject *parent) :
		Notifier("KNotify", tr("KDE4 notifications"), IconsManager::instance()->loadIcon("OpenChat"), parent)
{
	kdebugf();

	StripHTML.setPattern(QString::fromLatin1("<.*>"));
	StripHTML.setMinimal(true);

	KNotify = new QDBusInterface("org.kde.VisualNotifications",
			"/VisualNotifications", "org.kde.VisualNotifications");

	/* Dorr: maybe we're using patched version of KDE */
	if (!KNotify->isValid())
	{
		delete (KNotify);
		KNotify = new QDBusInterface("org.freedesktop.Notifications",
				"/org/freedesktop/Notifications", "org.freedesktop.Notifications");
	}

	KNotify->connection().connect(KNotify->service(), KNotify->path(), KNotify->interface(),
		"ActionInvoked", this, SLOT(actionInvoked(unsigned int, QString)));

	NotificationManager::instance()->registerNotifier(this);
	createDefaultConfiguration();

	kdebugf2();
}

KdeNotify::~KdeNotify()
{
	kdebugf();

	NotificationManager::instance()->unregisterNotifier(this);
	delete KNotify;
	KNotify = 0;

	kdebugf2();
}

void KdeNotify::createDefaultConfiguration()
{
	config_file.addVariable("KDENotify", "Timeout", 10);
	config_file.addVariable("KDENotify", "ShowContentMessage", true);
	config_file.addVariable("KDENotify", "CiteSign", 100);
}

void KdeNotify::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	connect(mainConfigurationWindow->widget()->widgetById("kdenotify/showContent"), SIGNAL(toggled(bool)),
			mainConfigurationWindow->widget()->widgetById("kdenotify/showContentCount"), SLOT(setEnabled(bool)));
}

void KdeNotify::notify(Notification *notification)
{
	QList<QVariant> args;
	args.append("Kadu");
	args.append(0U);
	args.append("");

	if (((notification->type() == "NewMessage") || (notification->type() == "NewChat")) &&
			config_file.readBoolEntry("KDENotify", "ShowContentMessage"))
	{
		args.append(notification->text().remove(StripHTML));
		QString strippedDetails = notification->details().remove(StripHTML);
		if (strippedDetails.length() > config_file.readNumEntry("KDENotify", "CiteSign", 10))
			args.append(strippedDetails.left(config_file.readNumEntry("KDENotify", "CiteSign", 10)) + "...");
		else
			args.append(strippedDetails);
	}
	else
	{
		args.append("Kadu");
		args.append(notification->text());
	}

	QStringList actions;

	foreach (Notification::Callback callback, notification->getCallbacks())
	{
		actions << callback.Signature;
		actions << callback.Caption;
	}

	args.append(actions);
	args.append(QVariantMap());
	args.append(config_file.readNumEntry("KDENotify", "Timeout", 10) * 1000);
	
	QDBusReply<unsigned int> reply = KNotify->callWithArgumentList(QDBus::Block, "Notify", args);
	if (reply.isValid())
	{
		notification->acquire(); // do not remove now

		connect(notification, SIGNAL(closed(Notification*)), this, SLOT(notificationClosed(Notification*)));

		NotificationMap.insert(reply.value(), notification);
		IdQueue.enqueue(reply.value());
		QTimer::singleShot(config_file.readNumEntry("KDENotify", "Timeout", 10) * 1000 + 2000, this, SLOT(deleteMapItem()));
	}
}

void KdeNotify::notificationClosed(Notification *notification)
{
	QMap<unsigned int, Notification *>::iterator i = NotificationMap.begin();
	while (i != NotificationMap.end())
	{
		if (i.value() == notification)
		{
			NotificationMap[i.key()] = 0;

			QList<QVariant> args;
			args.append(i.key());
			KNotify->callWithArgumentList(QDBus::Block, "CloseNotification", args);

			return;
		}
		i++;
	}
}

void KdeNotify::actionInvoked(unsigned int id, QString action)
{
	if (!IdQueue.contains(id))
		return;

	Notification *notification = NotificationMap.value(id);
	if (!notification)
		return;

	const QMetaObject *metaObject = notification->metaObject();
	int slotIndex;

	while (metaObject)
	{
		slotIndex = metaObject->indexOfSlot(action.toAscii().constData());
		if (slotIndex != -1)
			break;

		metaObject = metaObject->superClass();
	}

	if (-1 == slotIndex)
		return;

	QMetaMethod slot = notification->metaObject()->method(slotIndex);
	slot.invoke(notification, Qt::DirectConnection);

	QList<QVariant> args;
	args.append(id);
	KNotify->callWithArgumentList(QDBus::Block, "CloseNotification", args);

	NotificationMap[id] = 0;
}

void KdeNotify::deleteMapItem()
{
	unsigned int id = IdQueue.dequeue();
	Notification *notification = NotificationMap.value(id);
	NotificationMap.remove(id);

	if (notification)
		notification->release();
}

KdeNotify *kde_notify = 0;