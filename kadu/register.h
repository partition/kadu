#ifndef REGISTER_H
#define REGISTER_H

#include <qhbox.h>
#include <qtimer.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qsocketnotifier.h>

#include "gadu.h"

/**
	Dialog umożliwiający rejestrację nowego użytkownika
**/
class Register : public QHBox {
	Q_OBJECT

	public:
		Register(QDialog* parent = 0, const char *name = 0);
		~Register();

	private:
		QLineEdit *pwd, *pwd2, *mailedit;
		QLabel *status;
		UinType uin;
		QCheckBox *cb_updateconfig;

		void ask();	

	private slots:
		void doRegister();
		void keyPressEvent(QKeyEvent *);

	public slots:
		void registered(bool ok, UinType uin);

};

class Unregister : public QHBox {
	Q_OBJECT

	public:
		Unregister(QDialog* parent = 0, const char *name = 0);
		~Unregister();

	private:
		QLineEdit *uin, *pwd;
		QLabel *status;
		QCheckBox *updateconfig;

		void deleteConfig();

	private slots:
		void doUnregister();
		void keyPressEvent(QKeyEvent *);
	
	public slots:
		void unregistered(bool ok);

};

#endif

