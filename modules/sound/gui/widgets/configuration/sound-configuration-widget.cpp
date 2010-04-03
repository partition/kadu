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

#include <QtGui/QHBoxLayout>
#include <QtGui/QPushButton>

#include "configuration/configuration-file.h"
#include "gui/widgets/configuration/notify-group-box.h"
#include "gui/widgets/select-file.h"
#include "icons-manager.h"

#include "sound-manager.h"

#include "sound-configuration-widget.h"

SoundConfigurationWidget::SoundConfigurationWidget(QWidget *parent) :
		NotifierConfigurationWidget(parent), CurrentNotifyEvent("")
{
	QPushButton *testButton = new QPushButton(IconsManager::instance()->iconByPath("external_modules/module_mediaplayer_button-media-playback-start.png"),"", this);
	connect(testButton, SIGNAL(clicked()), this, SLOT(test()));

	SoundFileSelectFile = new SelectFile("audio", this);
	connect(SoundFileSelectFile, SIGNAL(fileChanged()), this, SIGNAL(soundFileEdited()));

	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->insertSpacing(0, 20);
	layout->addWidget(testButton);
	layout->addWidget(SoundFileSelectFile);

	dynamic_cast<NotifyGroupBox *>(parent)->addWidget(this);
}

SoundConfigurationWidget::~SoundConfigurationWidget()
{
}

void SoundConfigurationWidget::test()
{
	SoundManager::instance()->playFile(SoundFileSelectFile->file(), true);
}

void SoundConfigurationWidget::saveNotifyConfigurations()
{
	if (CurrentNotifyEvent != "")
		SoundFiles[CurrentNotifyEvent] = SoundFileSelectFile->file();

	foreach (const QString &key, SoundFiles.keys())
		config_file.writeEntry("Sounds", key + "_sound", SoundFiles[key]);
}

void SoundConfigurationWidget::switchToEvent(const QString &event)
{
	if (CurrentNotifyEvent != "")
		SoundFiles[CurrentNotifyEvent] = SoundFileSelectFile->file();
	CurrentNotifyEvent = event;

	if (SoundFiles.contains(event))
		SoundFileSelectFile->setFile(SoundFiles[event]);
	else
		SoundFileSelectFile->setFile(config_file.readEntry("Sounds", event + "_sound"));
}

void SoundConfigurationWidget::themeChanged(int index)
{
	if (index == 0)
		return;

	//refresh soundFiles
	foreach (const QString &key, SoundFiles.keys())
	{
		SoundFiles[key] = config_file.readEntry("Sounds", key + "_sound");
		if (key == CurrentNotifyEvent)
			SoundFileSelectFile->setFile(SoundFiles[key]);
	}
}