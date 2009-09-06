/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef FILE_TRANSFER_WINDOW
#define FILE_TRANSFER_WINDOW

#include <QtGui/QFrame>

#include "file_transfer.h"

class QLabel;
class QGridLayout;
class QProgressBar;
class QPushButton;
class QScrollArea;
class QVBoxLayout;

class FileTransferWindow;

class FileTransferWidget : public QFrame
{
	Q_OBJECT

	private:
		FileTransfer *ft;

		QLabel *description;
		QLabel *status;
		QProgressBar *progress;
		QPushButton *pauseButton;
		QPushButton *continueButton;

	private slots:
		void remove();
		void pauseTransfer();
		void continueTransfer();

	public:
		FileTransferWidget(QWidget *parent = 0, FileTransfer * = 0);
		virtual ~FileTransferWidget();

		FileTransfer *fileTransfer();

	public slots:
		void newFileTransfer(FileTransfer *);
		void fileTransferFailed(FileTransfer *, FileTransfer::FileTransferError);
		void fileTransferStatusChanged(FileTransfer *);
		void fileTransferFinished(FileTransfer *);
		void fileTransferDestroying(FileTransfer *);
};

class FileTransferWindow : public QFrame
{
	Q_OBJECT

	private:
		QScrollArea *scrollView;

		QFrame *frame;
		QVBoxLayout *transfersLayout;
		QMap<FileTransfer *, FileTransferWidget *> map;

	protected:
		virtual void keyPressEvent(QKeyEvent *e);
		void contentsChanged();

	public:
		FileTransferWindow(QWidget *parent = 0, const char *name = "file_transfer_window");
		virtual ~FileTransferWindow();

	private slots:
		void clearClicked();

	public slots:
		void newFileTransfer(FileTransfer *);
		void fileTransferFailed(FileTransfer *, FileTransfer::FileTransferError);
		void fileTransferStatusChanged(FileTransfer *);
		void fileTransferFinished(FileTransfer *);
		void fileTransferDestroying(FileTransfer *);
};

#endif