/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qlayout.h>
#include <qpushbutton.h>
#include <qstring.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qregexp.h>
#include <qtextcodec.h>
#include <stdlib.h>

#include "gadu.h"
#include "kadu.h"
#include "config_dialog.h"
#include "debug.h"
#include "events.h"
#include "message_box.h"
#include "ignore.h"
#include "expimp.h"
#include "status.h"

UserlistImport::UserlistImport(QWidget *parent, const char *name)
 : QDialog(parent, name, FALSE, Qt::WDestructiveClose) {
	results = new QListView(this);

	fetchbtn = new QPushButton(QIconSet(icons_manager.loadIcon("FetchUserlist")),tr("&Fetch userlist"), this);
	QObject::connect(fetchbtn, SIGNAL(clicked()), this, SLOT(startTransfer()));

	QPushButton *filebtn = new QPushButton(QIconSet(icons_manager.loadIcon("ImportFromFile")),tr("&Import from file"), this);
	QObject::connect(filebtn, SIGNAL(clicked()), this, SLOT(fromfile()));

	QPushButton *savebtn = new QPushButton(QIconSet(icons_manager.loadIcon("SaveUserlist")),tr("&Save results"), this);
	QObject::connect(savebtn, SIGNAL(clicked()), this, SLOT(makeUserlist()));

	QPushButton *mergebtn = new QPushButton(QIconSet(icons_manager.loadIcon("MergeUserlist")),tr("&Merge results"), this);
	QObject::connect(mergebtn, SIGNAL(clicked()), this, SLOT(updateUserlist()));

	results->addColumn(tr("UIN"));
	results->addColumn(tr("Nickname"));
	results->addColumn(tr("Disp. nick"));
	results->addColumn(tr("Name"));
	results->addColumn(tr("Surname"));
	results->addColumn(tr("Mobile no."));
	results->addColumn(tr("Group"));
	results->addColumn(tr("Email"));
	results->setAllColumnsShowFocus(true);

	QGridLayout * grid = new QGridLayout(this, 2, 3, 3, 3);
	grid->addMultiCellWidget(results, 0, 0, 0, 3);
	grid->addWidget(filebtn, 1, 0);	
	grid->addWidget(fetchbtn, 1, 1);
	grid->addWidget(savebtn, 1, 2);
	grid->addWidget(mergebtn, 1, 3);

	resize(450, 330);
	setCaption(tr("Import userlist"));	
}

void UserlistImport::readUserlist(QTextStream &stream) {
	kdebugf();
	UserListElement e;
	QListViewItem *qlv;
	QStringList sections, groupnames;
	QString line;
	int groups, i;
	bool ok;

	stream.setCodec(QTextCodec::codecForName("ISO 8859-2"));
	importedUserlist.clear();
	results->clear();
	while (!stream.eof()) {
		line = stream.readLine();
		sections = QStringList::split(";", line, true);
		if (sections.count() < 12)
			continue;
		if (sections[6] == "0")
			sections[6].truncate(0);
		e.first_name = sections[0];
		e.last_name = sections[1];
		e.nickname = sections[2];
		e.altnick = sections[3];
		e.mobile = sections[4];
		if (sections.count() >= 12)
			groups = sections.count() - 11;
		else
			groups = sections.count() - 7;
		groupnames.clear();
		for (i = 0; i < groups; i++)
			groupnames.append(sections[5 + i]);
		e.setGroup(groupnames.join(","));
		e.uin = sections[5 + groups].toUInt(&ok);
		if (!ok)
			e.uin = 0;
		e.description = QString::null;
		e.email = sections[6 + groups];
		importedUserlist.addUser(e);
		qlv = new QListViewItem(results, sections[5 + groups],
			sections[2], sections[3], sections[0],
			sections[1], sections[4], groupnames.join(","),
			sections[6 + groups]);
		}
}

void UserlistImport::fromfile() {
	kdebugf();
	QString fname = QFileDialog::getOpenFileName("/", QString::null, this);
	if (fname.length()) {
		QFile file(fname);
 		if (file.open(IO_ReadOnly)) {
			QTextStream stream(&file);
			readUserlist(stream);
			file.close();
			}
		else
			QMessageBox::critical(this, tr("Import error"),
				tr("The application encountered an internal error\nThe import userlist from file was unsuccessful"));
		}
}

void UserlistImport::startTransfer() {
	kdebugf();
	if (getCurrentStatus() == GG_STATUS_NOT_AVAIL)
		return;

	if (gg_userlist_request(sess, GG_USERLIST_GET, NULL) == -1) {
		kdebug("UserlistImport: gg_userlist_get() failed\n");
		QMessageBox::critical(this, tr("Import error"),
			tr("The application encountered an internal error\nThe import was unsuccessful"));
		return;
		}

	fetchbtn->setEnabled(false);

	importreply.truncate(0);
	connect(&event_manager, SIGNAL(userlistReplyReceived(char, char *)),
		this, SLOT(userlistReplyReceivedSlot(char, char *)));
}

void UserlistImport::closeEvent(QCloseEvent * e) {
	disconnect(&event_manager, SIGNAL(userlistReplyReceived(char, char *)),
		this, SLOT(userlistReplyReceivedSlot(char, char *)));
	QWidget::closeEvent(e);
}

void UserlistImport::makeUserlist() {
	unsigned int i;
	
	kdebugf();
	
	for (i=0; i < userlist.count(); i++)
		if (userlist[i].uin)
			gg_remove_notify(sess, userlist[i].uin);

	userlist = importedUserlist;
	
	clearIgnored();
	kadu->userbox()->clear();
	kadu->userbox()->clearUsers();
	for (i = 0; i < userlist.count(); i++)
		kadu->userbox()->addUser(userlist[i].altnick);
		
	UserBox::all_refresh();

	for (i = 0; i < userlist.count(); i++)
		if (userlist[i].uin)
			gg_add_notify(sess, userlist[i].uin);

	userlist.writeToFile();
	kdebug("UserlistImport::makeUserlist(): Wrote userlist\n");
}

void UserlistImport::updateUserlist() {
	unsigned int i;
	
	kdebugf();
	
	for (i=0; i < userlist.count(); i++)
		if (userlist[i].uin)
			gg_remove_notify(sess, userlist[i].uin);

	userlist.merge(importedUserlist);
	
	kadu->userbox()->clear();
	kadu->userbox()->clearUsers();
	for (i = 0; i < userlist.count(); i++)
		kadu->userbox()->addUser(userlist[i].altnick);
		
	UserBox::all_refresh();

	for (i = 0; i < userlist.count(); i++)
		if (userlist[i].uin)
			gg_add_notify(sess, userlist[i].uin);

	userlist.writeToFile();
	kdebug("UserlistImport::updateUserlist(): Wrote userlist\n");
}

void UserlistImport::userlistReplyReceivedSlot(char type, char *reply)
{
	kdebugf();
	if (type != GG_USERLIST_GET_REPLY && type != GG_USERLIST_GET_MORE_REPLY)
		return;

	if (!reply) {
		disconnect(&event_manager, SIGNAL(userlistReplyReceived(char, char *)),
			this, SLOT(userlistReplyReceivedSlot(char, char *)));
		results->clear();
		fetchbtn->setEnabled(true);
		return;
		}
	if (strlen(reply))
		importreply = importreply + cp2unicode((unsigned char *)reply);
	if (type == GG_USERLIST_GET_MORE_REPLY) {
		kdebug("ImportUserlist::userlistReplyReceived(): next portion\n");
		return;
		}
	fetchbtn->setEnabled(true);
	kdebug("ImportUserlist::userlistReplyReceivedSlot(): Done.\n");
	kdebug("ImportUserlist::userlistReplyReceivedSlot()\n%s\n",
		unicode2latin(importreply).data());
	QTextStream stream(&importreply, IO_ReadOnly);
	readUserlist(stream);
	disconnect(&event_manager, SIGNAL(userlistReplyReceived(char, char *)),
		this, SLOT(userlistReplyReceivedSlot(char, char *)));
}

UserlistExport::UserlistExport(QWidget *parent, const char *name)
 : QDialog (parent, name, FALSE, Qt::WDestructiveClose) {

	QGridLayout *grid = new QGridLayout(this,3,1,3,3);

	int i = 0;
	for (UserList::iterator it = userlist.begin(); it != userlist.end(); it++)
		if (!(*it).anonymous)
			i++;

	QString message(tr("%1 entries will be exported").arg(userlist.count()));

	QLabel *clabel = new QLabel(message,this);

	sendbtn = new QPushButton(QIconSet(icons_manager.loadIcon("SendUserlist")),tr("&Send userlist"),this);
	
	deletebtn = new QPushButton(QIconSet(icons_manager.loadIcon("DeleteUserlist")),tr("&Delete userlist"),this);
	
	tofilebtn = new QPushButton(QIconSet(icons_manager.loadIcon("ExportUserlist")),tr("&Export to file"),this);

	QPushButton * closebtn = new QPushButton(QIconSet(icons_manager.loadIcon("CloseWindow")),tr("&Close window"),this);

	QObject::connect(closebtn, SIGNAL(clicked()), this, SLOT(close()));

	grid->addWidget(clabel,0,0);
	grid->addWidget(sendbtn,1,0);
	grid->addWidget(deletebtn,2,0);	
	grid->addWidget(tofilebtn,3,0);		
	grid->addWidget(closebtn,4,0);

	QObject::connect(sendbtn, SIGNAL(clicked()), this, SLOT(startTransfer()));	
	QObject::connect(tofilebtn, SIGNAL(clicked()), this, SLOT(ExportToFile()));
	QObject::connect(deletebtn, SIGNAL(clicked()), this, SLOT(clean()));		

	setCaption(tr("Export userlist"));	
}

QString UserlistExport::saveContacts(){
	kdebugf();
	QString contacts, tmp;
	contacts="";
	for (unsigned int i=0; i < userlist.count(); i++)
		if (!userlist[i].anonymous) {
			contacts += userlist[i].first_name;
			contacts += ";";
			contacts += userlist[i].last_name;
			contacts += ";";
			contacts += userlist[i].nickname;
			contacts += ";";
			contacts += userlist[i].altnick;
			contacts += ";";
			contacts += userlist[i].mobile;
			contacts += ";";
			tmp = userlist[i].group();
			tmp.replace(QRegExp(","), ";");
			contacts += tmp;
			contacts += ";";
			if (userlist[i].uin)
				contacts += QString::number(userlist[i].uin);
			contacts += ";";
			contacts += userlist[i].email;
			contacts += ";0;;0;";
			contacts += "\r\n";
			}
	contacts.replace(QRegExp("(null)"), "");
	
	return contacts;
}

void UserlistExport::startTransfer() {
	kdebugf();
	if (getCurrentStatus() == GG_STATUS_NOT_AVAIL)
		return;

	QString contacts;
	contacts = saveContacts();

	char *con2;
	con2 = (char *)strdup((const char *)unicode2latin(contacts));
	kdebug("UserlistExport::startTransfer():\n%s\n", con2);
	free(con2);
	con2 = (char *)strdup((const char *)unicode2std(contacts));
	
	if (gg_userlist_request(sess, GG_USERLIST_PUT, con2) == -1) {
		kdebug("UserlistExport: gg_userlist_put() failed\n");
		QMessageBox::critical(this, tr("Export error"),
			tr("The application encountered an internal error\nThe export was unsuccessful"));
		free(con2);
		return;
		}
	free(con2);

	sendbtn->setEnabled(false);
	deletebtn->setEnabled(false);
	tofilebtn->setEnabled(false);	
	connect(&event_manager, SIGNAL(userlistReplyReceived(char, char *)),
		this, SLOT(userlistReplyReceivedSlot(char, char *)));
}

void UserlistExport::ExportToFile(void) {
	kdebugf();
	QString contacts;
	sendbtn->setEnabled(false);
	deletebtn->setEnabled(false);
	tofilebtn->setEnabled(false);	

	QString fname = QFileDialog::getSaveFileName("/", QString::null,this);
	if (fname.length()) {
		contacts = saveContacts();

		QFile file(fname);
		if (file.open(IO_WriteOnly)) {
			QTextStream stream(&file);
			stream.setCodec(QTextCodec::codecForName("ISO 8859-2"));
			stream << contacts;
			file.close();
			QMessageBox::information(this, tr("Export completed"),
				tr("Your userlist has been successfully exported to file"));
			}
		else
			QMessageBox::critical(this, tr("Export error"),
				tr("The application encountered an internal error\nThe export userlist to file was unsuccessful"));
		}

	sendbtn->setEnabled(true);
	deletebtn->setEnabled(true);
	tofilebtn->setEnabled(true);
}

void UserlistExport::clean() {
	kdebugf();
	if (getCurrentStatus() == GG_STATUS_NOT_AVAIL)
		return;

	const char *con2 = "";
	
	if (gg_userlist_request(sess, GG_USERLIST_PUT, con2) == -1) {
		kdebug("UserlistExport::clean(): Delete failed\n");
		QMessageBox::critical(this, tr("Export error"),
			tr("The application encountered an internal error\nThe delete userlist on server was unsuccessful"));
		return;
		}

	deletebtn->setEnabled(false);
	sendbtn->setEnabled(false);
	tofilebtn->setEnabled(false);
	connect(&event_manager, SIGNAL(userlistReplyReceived(char, char *)),
		this, SLOT(userlistReplyReceivedSlot(char, char *)));
}

void UserlistExport::userlistReplyReceivedSlot(char type, char *reply) {
	kdebug("ExportUserlist::userlistReplyReceivedSlot()\n");
	if (type != GG_USERLIST_PUT_REPLY)
		return;
	kdebug("ExportUserlist::userlistReplyReceivedSlot(): Done\n");
	
	disconnect(&event_manager, SIGNAL(userlistReplyReceived(char, char *)),
		this, SLOT(userlistReplyReceivedSlot(char, char *)));
	sendbtn->setEnabled(true);
	tofilebtn->setEnabled(true);
	deletebtn->setEnabled(true);

	MessageBox::msg(tr("Your userlist has been successfully exported to server"));
}

void UserlistExport::closeEvent(QCloseEvent * e) {
	disconnect(&event_manager, SIGNAL(userlistReplyReceived(char, char *)),
		this, SLOT(userlistReplyReceivedSlot(char, char *)));
	QWidget::closeEvent(e);
}

