/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QFileInfo>
#include <QtCore/QTimer>
#include <QtGui/QApplication>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>

#include "buddies/buddy.h"
#include "file-transfer/file-transfer.h"
#include "file-transfer/file-transfer-handler.h"
#include "file-transfer/file-transfer-manager.h"
#include "file-transfer/file-transfer-shared.h"
#include "gui/windows/message-dialog.h"

#include "debug.h"
#include "icons-manager.h"

#include "file-transfer-widget.h"

FileTransferWidget::FileTransferWidget(FileTransfer ft, QWidget *parent)
	: QFrame(parent), CurrentTransfer(ft), Speed(0)
{
	kdebugf();
	
	createGui();
	
	LastTransferredSize = CurrentTransfer.transferredSize();
	connect(CurrentTransfer, SIGNAL(updated()), this, SLOT(fileTransferUpdate()));
	fileTransferUpdate();

	show();
}

FileTransferWidget::~FileTransferWidget()
{
	kdebugf();

	disconnect(CurrentTransfer, SIGNAL(updated()), this, SLOT(fileTransferUpdate()));
}

void FileTransferWidget::createGui()
{
	setBackgroundRole(QPalette::Base);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	setMinimumSize(QSize(100, 100));

	setFrameStyle(QFrame::Box | QFrame::Sunken);
	setLineWidth(1);

	QGridLayout *layout = new QGridLayout(this);
	layout->setMargin(10);
	layout->setColumnStretch(0, 1);
	layout->setColumnStretch(1, 20);
	layout->setColumnStretch(2, 20);

	QLabel *icon = new QLabel(this);
	icon->setBackgroundRole(QPalette::Base);
	layout->addWidget(icon, 0, 0, 3, 1);

	DescriptionLabel = new QLabel(this);
	DescriptionLabel->setBackgroundRole(QPalette::Base);
	DescriptionLabel->setScaledContents(true);
	layout->addWidget(DescriptionLabel, 0, 1, 1, 2);

	ProgressBar = new QProgressBar;
	ProgressBar->setMinimum(0);
	ProgressBar->setMaximum(100);
	ProgressBar->setBackgroundRole(QPalette::Base);
	layout->addWidget(ProgressBar, 1, 1, 1, 2);

	StatusLabel = new QLabel(this);
	StatusLabel->setBackgroundRole(QPalette::Base);
	layout->addWidget(StatusLabel, 2, 1);

	QWidget *buttons = new QWidget;
	QHBoxLayout *buttons_layout = new QHBoxLayout;
	buttons->setBackgroundRole(QPalette::Base);
	buttons_layout->setSpacing(2);

	PauseButton = new QPushButton(tr("Pause"), this);
	PauseButton->hide();
	connect(PauseButton, SIGNAL(clicked()), this, SLOT(pauseTransfer()));

	ContinueButton = new QPushButton(tr("Continue"), this);
	ContinueButton->hide();
	connect(ContinueButton, SIGNAL(clicked()), this, SLOT(continueTransfer()));

	QPushButton *deleteThis = new QPushButton(tr("Remove"), this);
	connect(deleteThis, SIGNAL(clicked()), this, SLOT(removeTransfer()));

	buttons_layout->addWidget(PauseButton);
	buttons_layout->addWidget(ContinueButton);
	buttons_layout->addWidget(deleteThis);
	buttons->setLayout(buttons_layout);
 	layout->addWidget(buttons, 2, 2, Qt::AlignRight);

	Buddy buddy = CurrentTransfer.fileTransferContact().ownerBuddy();

	QString fileName = QFileInfo(CurrentTransfer.localFileName()).fileName();
	if (fileName.isEmpty())
		fileName = CurrentTransfer.remoteFileName();

	if (TypeSend == CurrentTransfer.transferType())
	{
		icon->setPixmap(IconsManager::instance()->loadPixmap("FileTransferSend"));
		DescriptionLabel->setText(tr("<b>File</b> %1 <b>to</b> %2").arg(fileName).arg(buddy.display()));
	}
	else
	{
		icon->setPixmap(IconsManager::instance()->loadPixmap("FileTransferReceive"));
		DescriptionLabel->setText(tr("<b>File</b> %1 <b>from</b> %2").arg(fileName).arg(buddy.display()));
	}
}

FileTransferHandler * FileTransferWidget::handler()
{
	return CurrentTransfer.handler();
}

void FileTransferWidget::removeTransfer()
{
	kdebugf();

	if (!CurrentTransfer)
		return;

	if (StatusFinished != CurrentTransfer.transferStatus())
		if (!MessageDialog::ask(tr("Are you sure you want to remove this transfer?"), QString::null, this))
			return;
		else
		{
			if (handler())
				handler()->stop();
		}

	FileTransferManager::instance()->removeItem(CurrentTransfer);

	deleteLater();
}

void FileTransferWidget::pauseTransfer()
{
	kdebugf();

	if (handler())
		handler()->pause();
}

void FileTransferWidget::continueTransfer()
{
	kdebugf();

	if (handler())
		handler()->restore();
}

void FileTransferWidget::fileTransferUpdate()
{
	if (!CurrentTransfer)
	{
		StatusLabel->setText(tr("<b>Not connected</b>"));
		PauseButton->hide();
		ContinueButton->show();
		return;
	}

	if (ErrorOk != CurrentTransfer.transferError())
	{
		StatusLabel->setText(tr("<b>Error</b>"));
		PauseButton->hide();
		ContinueButton->show();
		return;
	}

	if (StatusFinished != CurrentTransfer.transferStatus())
		ProgressBar->setValue(CurrentTransfer.percent());
	else
		ProgressBar->setValue(100);

	if (StatusTransfer == CurrentTransfer.transferStatus())
	{
		if (LastUpdateTime.isValid())
		{
			QDateTime now = QDateTime::currentDateTime();
			int timeDiff = now.toTime_t() - LastUpdateTime.toTime_t();
			if (0 < timeDiff)
			{
				Speed = ((CurrentTransfer.transferredSize() - LastTransferredSize) / 1024) / timeDiff;
				LastUpdateTime = QDateTime::currentDateTime();
				LastTransferredSize = CurrentTransfer.transferredSize();
			}
		}
		else
		{
			Speed = 0;
			LastUpdateTime = QDateTime::currentDateTime();
			LastTransferredSize = CurrentTransfer.transferredSize();
		}
	}

	switch (CurrentTransfer.transferStatus())
	{
		case StatusNotConnected:
			StatusLabel->setText(tr("<b>Not connected</b>"));
			PauseButton->hide();
			ContinueButton->show();
			break;

		case StatusWaitingForConnection:
			StatusLabel->setText(tr("<b>Wait for connection</b>"));
			break;

		case StatusWaitingForAccept:
			StatusLabel->setText(tr("<b>Wait for accept</b>"));
			break;

		case StatusTransfer:
			StatusLabel->setText(tr("<b>Transfer</b>: %1 kB/s").arg(QString::number(Speed)));
			PauseButton->show();
			ContinueButton->hide();
			break;

		case StatusFinished:
			StatusLabel->setText(tr("<b>Finished</b>"));
			PauseButton->hide();
			ContinueButton->hide();
			break;

		case StatusRejected:
			StatusLabel->setText(tr("<b>Rejected</b>"));
			PauseButton->hide();
			ContinueButton->hide();
			break;

		default:
			PauseButton->hide();
			ContinueButton->hide();
	}

	qApp->processEvents();
}