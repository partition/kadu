/***************************************************************************
                          ignore.h  -  description
                             -------------------
    begin                : Sat Nov 3 2001
    copyright            : (C) 2001 by tomee
    email                : tomee@cpi.pl
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef IGNORE_H
#define IGNORE_H

#include <qdialog.h>
#include <qlistbox.h>
#include <qlineedit.h>
#include "../libgadu/lib/libgadu.h"

class Ignored : public QDialog {
	Q_OBJECT
	public:
		Ignored(QDialog* parent=0, const char *name=0);

	private:
		QListBox * list;
		QLineEdit * e_uin;

	private slots:
		void remove();
		void add();
		void getList();	
};

#endif
