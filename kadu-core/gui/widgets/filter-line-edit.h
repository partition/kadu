/*
 * %kadu copyright begin%
 * Copyright 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
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

#ifndef FILTER_LINE_EDIT
#define FILTER_LINE_EDIT

#include <QLabel>
#include <QLineEdit>
#include <QPainter>
#include <QPaintEvent>
#include <QTimeLine>
#include <QIcon>

class LineEditClearButton : public QWidget
{
	Q_OBJECT
	
	QTimeLine *Timeline;
	QPixmap ButtonPixmap;
	QIcon ButtonIcon;

public:
	LineEditClearButton(QWidget *parent);
	
	QSize sizeHint() const { return ButtonPixmap.size(); }
	void animateVisible(bool visible);
	void setPixmap(const QPixmap& p);
	QPixmap pixmap() const { return ButtonPixmap; }
	void setAnimationsEnabled(bool animationsEnabled);

protected:
	void paintEvent(QPaintEvent *event);
	virtual bool event(QEvent* event );
    
protected slots:
	void animationFinished();
};


class FilterLineEdit : public QLineEdit
{
	Q_OBJECT
    
    	LineEditClearButton *ClearFilterButton;
	
	bool WideEnoughForClear;
	int Overlap;
	bool ClickInClear;
	
	void updateClearButton();

public:
	FilterLineEdit(QWidget *parent);
	
private slots:
	void updateClearButtonIcon(const QString& text);
	
protected:
	virtual void mousePressEvent( QMouseEvent* e );
	virtual void mouseReleaseEvent( QMouseEvent* e );
};


#endif
