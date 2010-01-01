/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef BUDDIES_LIST_WIDGET_H
#define BUDDIES_LIST_WIDGET_H

#include <QtGui/QWidget>

class QLineEdit;

class BuddyNameFilter;
class BuddiesListView;
class MainWindow;

class BuddiesListWidget : public QWidget
{
	Q_OBJECT

public:
	enum FilterPosition
	{
		FilterAtTop,
		FilterAtBottom
	};

private:
	QLineEdit *NameFilterEdit;
	BuddiesListView *View;
	BuddyNameFilter *NameFilter;

private slots:
	void nameFilterChanged(const QString &filter);

protected:
	virtual bool eventFilter(QObject *, QEvent *);

public:
	explicit BuddiesListWidget(FilterPosition filterPosition, MainWindow *mainWindow, QWidget *parent = 0);
	virtual ~BuddiesListWidget();

	BuddiesListView * view() { return View; }
	QLineEdit * nameFilterEdit() { return NameFilterEdit; }
	BuddyNameFilter * nameFilter() { return NameFilter; }

};

#endif //BUDDIES_LIST_WIDGET_H