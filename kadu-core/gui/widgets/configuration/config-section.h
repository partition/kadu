/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#ifndef CONFIG_SECTION_H
#define CONFIG_SECTION_H

#include <QtCore/QObject>
#include <QtCore/QMap>

#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/widgets/configuration/kadu-tab-widget.h"

class ConfigGroupBox;
class ConfigTab;
class ConfigurationWidget;
class ConfigWidget;

class QGridLayout;
class QGroupBox;
class QListWidgetItem;

class ConfigSection : public QObject
{
	Q_OBJECT

	QString Name;
	ConfigurationWidget *MyConfigurationWidget;
	QString IconPath;

	QListWidgetItem *ListWidgetItem;
	bool Activated;
	QMap<QString, ConfigTab *> ConfigTabs;

	QWidget *ParentConfigGroupBoxWidget;
	KaduTabWidget *TabWidget;

	ConfigTab * configTab(const QString &name, bool create = true);

private slots:
	void configTabDestroyed(QObject *obj);
	void iconThemeChanged();

public:
	ConfigSection(const QString &name, ConfigurationWidget *configurationWidget, QListWidgetItem *listWidgetItem, QWidget *parentConfigGroupBoxWidget,
		const QString &iconPath);
	virtual ~ConfigSection();

	const QString & name() const { return Name; }

	void activate();

	void show() { TabWidget->show(); }
	void hide() { TabWidget->hide(); }

	ConfigGroupBox * configGroupBox(const QString &tab, const QString &groupBox, bool create = true);

};


#endif
