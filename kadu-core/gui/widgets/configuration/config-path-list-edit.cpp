/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QApplication>
#include <QtGui/QLabel>

#include "gui/widgets/configuration/config-path-list-edit.h"
#include "gui/widgets/configuration/config-group-box.h"
#include "configuration/configuration-window-data-manager.h"

#include "debug.h"

ConfigPathListEdit::ConfigPathListEdit(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip,
		ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager)
	: PathListEdit(parentConfigGroupBox->widget()), ConfigWidgetValue(section, item, widgetCaption, toolTip, parentConfigGroupBox, dataManager), label(0)
{
	createWidgets();
}

ConfigPathListEdit::ConfigPathListEdit(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager)
	: PathListEdit(parentConfigGroupBox->widget()), ConfigWidgetValue(parentConfigGroupBox, dataManager), label(0)
{
}

ConfigPathListEdit::~ConfigPathListEdit()
{
	if (label)
		delete label;
}

void ConfigPathListEdit::createWidgets()
{
	kdebugf();

	label = new QLabel(qApp->translate("@default", widgetCaption.toAscii().data()) + ':', parentConfigGroupBox->widget());
	parentConfigGroupBox->addWidgets(label, this);

	if (!ConfigWidget::toolTip.isEmpty())
	{
		setToolTip(qApp->translate("@default", ConfigWidget::toolTip.toAscii().data()));
		label->setToolTip(qApp->translate("@default", ConfigWidget::toolTip.toAscii().data()));
	}
}

void ConfigPathListEdit::loadConfiguration()
{
	if (!dataManager)
		return;
	setPathList(dataManager->readEntry(section, item).toString().split(QRegExp("&"), QString::SkipEmptyParts));
}

void ConfigPathListEdit::saveConfiguration()
{
	if (!dataManager)
		return;
	dataManager->writeEntry(section, item, QVariant(pathList().join("&")));
}

void ConfigPathListEdit::show()
{
	label->show();
	PathListEdit::show();
}

void ConfigPathListEdit::hide()
{
	label->hide();
	PathListEdit::hide();
}
