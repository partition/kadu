/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QKeyEvent>
#include <QtGui/QPushButton>
#include <QtGui/QScrollArea>

#include "file-transfer/file-transfer.h"
#include "file-transfer/file-transfer-manager.h"

#include "debug.h"
#include "misc/misc.h"

#include "gui/widgets/file-transfer-widget.h"

#include "file-transfer-window.h"

FileTransferWindow::FileTransferWindow(QWidget *parent) :
	QFrame(parent)
{
	kdebugf();

	setWindowRole("kadu-file-transfer");

	createGui();
	loadWindowGeometry(this, "General", "TransferWindowGeometry", 200, 200, 500, 300);

	foreach (FileTransfer fileTransfer, FileTransferManager::instance()->items())
			fileTransferAdded(fileTransfer);
	connect(FileTransferManager::instance(), SIGNAL(fileTransferAdded(FileTransfer)),
			this, SLOT(fileTransferAdded(FileTransfer)));
	connect(FileTransferManager::instance(), SIGNAL(fileTransferRemoved(FileTransfer)),
			this, SLOT(fileTransferRemoved(FileTransfer)));

	contentsChanged();

	kdebugf2();
}

FileTransferWindow::~FileTransferWindow()
{
	kdebugf();

	disconnect(FileTransferManager::instance(), SIGNAL(fileTransferAdded(FileTransfer)),
			this, SLOT(fileTransferAdded(FileTransfer)));
	disconnect(FileTransferManager::instance(), SIGNAL(fileTransferRemoved(FileTransfer)),
			this, SLOT(fileTransferRemoved(FileTransfer)));

	saveWindowGeometry(this, "General", "TransferWindowGeometry");

	kdebugf2();
}

void FileTransferWindow::createGui()
{
	setWindowTitle(tr("Kadu - file transfers"));
	setMinimumSize(QSize(100, 100));

	setAttribute(Qt::WA_DeleteOnClose);

	QGridLayout *mainGrid = new QGridLayout(this);
	mainGrid->setSpacing(2);
	mainGrid->setMargin(2);

	ScrollView = new QScrollArea(this);
// 	scrollView->setResizePolicy(QScrollArea::AutoOneFit);

	mainGrid->addWidget(ScrollView, 0, 0);
	ScrollView->move(0, 0);

	InnerFrame = new QFrame(this);
 	InnerFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

	TransfersLayout = new QVBoxLayout(InnerFrame);
	TransfersLayout->setDirection(QBoxLayout::Up);

 	ScrollView->setWidget(InnerFrame);
	ScrollView->setWidgetResizable(true);

	QWidget *buttonBox = new QWidget;
	QHBoxLayout *buttonBox_layout = new QHBoxLayout;
	buttonBox_layout->setContentsMargins(2, 2, 2, 2);
	buttonBox_layout->setSpacing(2);

	QPushButton *cleanButton = new QPushButton(tr("Clear"), this);
	connect(cleanButton, SIGNAL(clicked()), this, SLOT(clearClicked()));

	QPushButton *hideButton = new QPushButton(tr("Hide"), this);
	connect(hideButton, SIGNAL(clicked()), this, SLOT(close()));
	buttonBox_layout->addWidget(cleanButton);
	buttonBox_layout->addWidget(hideButton);
	buttonBox->setLayout(buttonBox_layout);
	mainGrid->addWidget(buttonBox, 1, 0, Qt::AlignRight);

}

void FileTransferWindow::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_Escape)
	{
		close();
		e->accept();
	}
	else
		QFrame::keyPressEvent(e);
}

void FileTransferWindow::fileTransferAdded(FileTransfer fileTransfer)
{
	FileTransferWidget *ftm = new FileTransferWidget(fileTransfer, InnerFrame);
	TransfersLayout->addWidget(ftm);
	Widgets.append(ftm);

	contentsChanged();
}

void FileTransferWindow::fileTransferRemoved(FileTransfer fileTransfer)
{
	foreach (FileTransferWidget *ftm, Widgets)
		if (ftm->fileTransfer() == fileTransfer)
		{
			ftm->deleteLater();
			contentsChanged();
			Widgets.removeAll(ftm);
			return;
		}
}

void FileTransferWindow::clearClicked()
{
	FileTransferManager::instance()->cleanUp();
}

void FileTransferWindow::contentsChanged()
{
	kdebugf();

	TransfersLayout->invalidate();
}
