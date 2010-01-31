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

#include <QtGui/QFileDialog>
#include <QtGui/QLabel>
#include <QtGui/QMovie>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>

#include "buddies/avatar.h"
#include "protocols/services/avatar-service.h"
#include "protocols/protocol.h"
#include "icons-manager.h"

#include "account-avatar-widget.h"

AccountAvatarWidget::AccountAvatarWidget(Account account, QWidget *parent) :
		QWidget(parent), MyAccount(account), Service(0), WaitMovie(0)
{

	QString pleaseWaithPath = IconsManager::instance()->iconPath("PleaseWait");
	WaitMovie = new QMovie(pleaseWaithPath);

	createGui();

	triggerAllProtocolsRegistered();
}

AccountAvatarWidget::~AccountAvatarWidget()
{
	if (WaitMovie)
		delete WaitMovie;
}

void AccountAvatarWidget::createGui()
{
	QVBoxLayout *layout = new QVBoxLayout(this);

	AvatarLabel = new QLabel();
	AvatarLabel->setAlignment(Qt::AlignCenter);
	AvatarLabel->setMaximumSize(128, 128);
	AvatarLabel->setScaledContents(true);

	Avatar avatar = MyAccount.accountContact().contactAvatar();
	if (avatar)
		connect(avatar, SIGNAL(updated()), this, SLOT(avatarUpdated()));
	avatarUpdated();

	layout->addWidget(AvatarLabel);

	ChangeAvatarButton = new QPushButton(tr("Change avatar..."), this);
	connect(ChangeAvatarButton, SIGNAL(clicked(bool)), this, SLOT(changeAvatar()));

	layout->addWidget(ChangeAvatarButton, 0, Qt::AlignHCenter);
}

void AccountAvatarWidget::protocolRegistered(ProtocolFactory *protocolFactory)
{
	if (Service)
		disconnect(Service, SIGNAL(avatarUploaded(bool, QImage)), this, SLOT(avatarUploaded(bool, QImage)));

	Protocol *protocol = MyAccount.protocolHandler();
	if (!protocol)
		return;

	Service = protocol->avatarService();
	setEnabled(0 != Service);

	if (Service)
		connect(Service, SIGNAL(avatarUploaded(bool, QImage)), this, SLOT(avatarUploaded(bool, QImage)));
}

void AccountAvatarWidget::protocolUnregistered(ProtocolFactory *protocolFactory)
{
	protocolRegistered(protocolFactory);
}

void AccountAvatarWidget::avatarUpdated()
{
	WaitMovie->stop();
	AvatarLabel->setMovie(0);
	AvatarLabel->setScaledContents(true);
	AvatarLabel->setPixmap(MyAccount.accountContact().contactAvatar().pixmap());
}

void AccountAvatarWidget::changeAvatar()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Select avatar file"), "", "Images (*.jpeg *.jpg *.png)");
	if (fileName.isEmpty())
		return;

	QImage avatar(fileName);
	if (avatar.isNull())
		return;

	AvatarLabel->setScaledContents(false);
	AvatarLabel->setMovie(WaitMovie);
	WaitMovie->start();

	Service->uploadAvatar(avatar);
	ChangeAvatarButton->setEnabled(false);
}

void AccountAvatarWidget::avatarUploaded(bool ok, QImage image)
{
	if (ok)
		MyAccount.accountContact().contactAvatar().setPixmap(QPixmap::fromImage(image));

	avatarUpdated();
	ChangeAvatarButton->setEnabled(true);
}