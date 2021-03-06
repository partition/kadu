/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef IMPORT_PROFILES_WINDOW_H
#define IMPORT_PROFILES_WINDOW_H

#include <QtCore/QMap>
#include <QtGui/QDialog>

class QCheckBox;
class QGridLayout;

struct ProfileData;

class ImportProfilesWindow : public QDialog
{
	Q_OBJECT

	QMap<QCheckBox *, ProfileData> ProfileCheckBoxes;
	QMap<QCheckBox *, QCheckBox *> HistoryCheckBoxes;

	void createGui();
	void createProfileList(QGridLayout *formLayout);

public:
	explicit ImportProfilesWindow(QWidget *parent = 0);
	virtual ~ImportProfilesWindow();

	virtual void accept();

};

#endif // IMPORT_PROFILES_WINDOW_H
