/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CHAT_H
#define CHAT_H

#include <qdialog.h>
//#include <ktextbrowser.h>
#include <qtextbrowser.h>
#include <qhbox.h>
#include <qmultilineedit.h>
#include <qpushbutton.h>
#include <qarray.h>
#include <qtoolbutton.h>
#include "misc.h"
#include "userbox.h"
#include "../config.h"

class IconSelector;

class CustomInput;

class KaduTextBrowser : public QTextBrowser {
	public:
		KaduTextBrowser(QWidget *parent = 0, const char *name = 0);
		void setSource(const QString &name);
};

class Chat : public QWidget {
	Q_OBJECT
		int index;
		int totaloccurences;
		UinsList uins;
		IconSelector *iconsel_ptr;
		QPushButton *iconsel;
		QPushButton *autosend;
		QPushButton *lockscroll;
		QAccel *acc;
		QPushButton *sendbtn;
		QPushButton *cancelbtn;
		UserBox *userbox;
		QString myLastMessage;
#ifdef HAVE_OPENSSL
		QPushButton *encryption;
#endif

		void pruneWindow(void);

	public:
		Chat(UinsList uins, QWidget *parent = 0, const char *name = 0);
		~Chat();
		void changeAppearance();
		void setTitle(void);
		void formatMessage(bool, QString &, QString &, const char *, QString &);
		void checkPresence(UinsList, QString &, time_t, QString &);
		void addEmoticon(QString);
		void scrollMessages(QString &);
		void alertNewMessage(void);
      
		QTextBrowser *body;
		CustomInput *edit;
		QHBox *buttontray;
		bool autosend_enabled;
#ifdef HAVE_OPENSSL
		bool encrypt_enabled;
#endif
    
	public slots:
		void HistoryBox(void);
		void sendMessage(void);
		void cancelMessage(void);
		void writeMyMessage(void);
		void changeTitle(void);
		void hyperlinkClicked(const QString &link);

	protected:
		void closeEvent(QCloseEvent *);
		QString convertCharacters(QString);
		virtual void windowActivationChange(bool oldActive);
		void keyPressEvent(QKeyEvent *e);

	private slots:
		void userWhois(void);
		void insertEmoticon(void);
		void regEncryptSend(void);
		void regAutosend(void);
		void addMyMessageToHistory(void);
		void clearChatWindow(void);

	private:
		QString title_buffer;
		QTimer *title_timer;  
};

class CustomInput : public QMultiLineEdit {
	Q_OBJECT
	public:
		CustomInput(QWidget *parent = 0, Chat *owner = 0, const char *name = 0);
		friend class Chat;

	private:
		Chat *tata;

	protected:
		void keyPressEvent(QKeyEvent *e);
};

class IconSelectorButton : public QToolButton
{
	Q_OBJECT
	
	private:
		QString EmoticonString;
	
	private slots:
		void buttonClicked();

	public:
		IconSelectorButton(QWidget* parent,const QString& emoticon_string);	
	
	signals:
		void clicked(const QString& emoticon_string);
};

class IconSelector : public QWidget {
	Q_OBJECT
	public:
		IconSelector(QWidget* parent = 0, const char *name = 0, Chat *caller = 0);

	private:
		Chat *callingwidget;

	protected:
		void closeEvent(QCloseEvent *e);

	private slots:
		void iconClicked(const QString& emoticon_string);
};

#endif
