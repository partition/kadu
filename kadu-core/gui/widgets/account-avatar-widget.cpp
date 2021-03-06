/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtGui/QFileDialog>
#include <QtGui/QLabel>
#include <QtGui/QMovie>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>

#include "avatars/avatar.h"
#include "avatars/avatar-manager.h"
#include "protocols/services/avatar-service.h"
#include "protocols/protocol.h"
#include "protocols/protocol-factory.h"
#include "icons-manager.h"

#include "account-avatar-widget.h"

AccountAvatarWidget::AccountAvatarWidget(Account account, QWidget *parent) :
		QWidget(parent), MyAccount(account), Service(0), WaitMovie(0)
{

	QString pleaseWaithPath = IconsManager::instance()->iconPath("kadu_icons/16x16/please-wait.gif");
	WaitMovie = new QMovie(pleaseWaithPath, QByteArray(), this);

	createGui();

	triggerAllProtocolsRegistered();
}

AccountAvatarWidget::~AccountAvatarWidget()
{
}

void AccountAvatarWidget::serviceDestroyed()
{
	Service = 0;
}

void AccountAvatarWidget::createGui()
{
	QVBoxLayout *layout = new QVBoxLayout(this);

	AvatarLabel = new QLabel();
	AvatarLabel->setAlignment(Qt::AlignCenter);
	AvatarLabel->setFixedWidth(128);

	layout->addWidget(AvatarLabel);

	ChangePhotoButton = new QPushButton(this);
	connect(ChangePhotoButton, SIGNAL(clicked(bool)), this, SLOT(changeButtonClicked()));
	setupMode();

	layout->addWidget(ChangePhotoButton, 0, Qt::AlignHCenter);

	Avatar avatar = MyAccount.accountContact().contactAvatar();
	if (avatar)
		connect(avatar, SIGNAL(updated()), this, SLOT(avatarUpdated()));
	avatarUpdated();
}

void AccountAvatarWidget::setupMode()
{
	if (MyAccount.protocolHandler()->protocolFactory()->canRemoveAvatar() && !MyAccount.accountContact().contactAvatar().isEmpty())
		Mode = ModeRemove;
	else
		Mode = ModeChange;

	if (ModeRemove == Mode)
		ChangePhotoButton->setText(tr("Remove Photo..."));
	else
		ChangePhotoButton->setText(tr("Change Photo..."));
}

void AccountAvatarWidget::protocolRegistered(ProtocolFactory *protocolFactory)
{
	Q_UNUSED(protocolFactory)

	if (Service)
	{
		disconnect(Service, SIGNAL(destroyed()), this, SLOT(serviceDestroyed()));
		disconnect(Service, SIGNAL(avatarUploaded(bool, QImage)), this, SLOT(avatarUploaded(bool, QImage)));
	}

	Protocol *protocol = MyAccount.protocolHandler();
	if (!protocol)
		return;

	Service = protocol->avatarService();
	setEnabled(0 != Service);

	if (Service)
	{
		connect(Service, SIGNAL(avatarUploaded(bool, QImage)), this, SLOT(avatarUploaded(bool, QImage)));
		connect(Service, SIGNAL(destroyed()), this, SLOT(serviceDestroyed()));
	}
}

void AccountAvatarWidget::protocolUnregistered(ProtocolFactory *protocolFactory)
{
	protocolRegistered(protocolFactory);
}

void AccountAvatarWidget::avatarUpdated()
{
	WaitMovie->stop();
	AvatarLabel->setMovie(0);
	QPixmap avatar = MyAccount.accountContact().contactAvatar().pixmap();

	if (avatar.width() > 128 || avatar.height() > 128)
		avatar = avatar.scaled(QSize(128, 128), Qt::KeepAspectRatio, Qt::SmoothTransformation);
	AvatarLabel->setPixmap(avatar);

	setupMode();
}

void AccountAvatarWidget::changeButtonClicked()
{
	if (ModeRemove == Mode)
		removeAvatar();
	else
		changeAvatar();
}

void AccountAvatarWidget::changeAvatar()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Select avatar file"), QString(), "Images (*.jpeg *.jpg *.png)");
	if (fileName.isEmpty())
		return;

	QImage avatar(fileName);
	if (avatar.isNull())
		return;

	AvatarLabel->setScaledContents(false);
	AvatarLabel->setMovie(WaitMovie);
	WaitMovie->start();

	Service->uploadAvatar(avatar);
	ChangePhotoButton->setEnabled(false);
}

void AccountAvatarWidget::removeAvatar()
{
	AvatarLabel->setScaledContents(false);
	AvatarLabel->setMovie(WaitMovie);
	WaitMovie->start();

	Service->uploadAvatar(QImage());
	ChangePhotoButton->setEnabled(false);
}

void AccountAvatarWidget::avatarUploaded(bool ok, QImage image)
{
	if (ok)
		AvatarManager::instance()->byContact(MyAccount.accountContact(), ActionCreateAndAdd).setPixmap(QPixmap::fromImage(image));

	avatarUpdated();
	ChangePhotoButton->setEnabled(true);
}
