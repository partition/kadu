/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QApplication>
#include <QtGui/QHeaderView>
#include <QtGui/QPainter>
#include <QtGui/QTextDocument>
#include <QtGui/QTextFrame>
#include <QtGui/QTextFrameFormat>
#include <QtGui/QTreeView>

#include "buddies/buddy.h"
#include "chat/message/pending-messages-manager.h"
#include "contacts/contact.h"
#include "gui/widgets/buddies-list-view-avatar-painter.h"
#include "gui/widgets/buddies-list-view-delegate-configuration.h"
#include "model/roles.h"

#include "buddies-list-view-item-painter.h"

BuddiesListViewItemPainter::BuddiesListViewItemPainter(const BuddiesListViewDelegateConfiguration &configuration, const QStyleOptionViewItemV4 &option, const QModelIndex &index) :
		Configuration(configuration), Option(option), Index(index),
		FontMetrics(Configuration.font()),
		BoldFontMetrics(Configuration.boldFont()),
		DescriptionFontMetrics(Configuration.descriptionFont()),
		DescriptionDocument(0)
{
	Widget = dynamic_cast<const QTreeView *>(option.widget);

	QStyle *style = Widget->style();

	HFrameMargin = style->pixelMetric(QStyle::PM_FocusFrameHMargin, 0, Widget);
	VFrameMargin = style->pixelMetric(QStyle::PM_FocusFrameVMargin, 0, Widget);
}

BuddiesListViewItemPainter::~BuddiesListViewItemPainter()
{
	delete DescriptionDocument;
	DescriptionDocument = 0;
}

bool BuddiesListViewItemPainter::useBold() const
{
	if (!Configuration.showBold())
		return false;

	QVariant statVariant = Index.data(StatusRole);
	if (!statVariant.canConvert<Status>())
		return false;

	Status status = statVariant.value<Status>();
	return !status.isDisconnected();
}

bool BuddiesListViewItemPainter::showMessagePixmap() const
{
	if (Index.parent().isValid()) // contact
	{
		Contact contact = qvariant_cast<Contact>(Index.data(ContactRole));
		return contact && PendingMessagesManager::instance()->hasPendingMessagesForContact(contact);
	}
	else
	{
		Buddy buddy = qvariant_cast<Buddy>(Index.data(BuddyRole));
		return buddy && PendingMessagesManager::instance()->hasPendingMessagesForBuddy(buddy);
	}
}

bool BuddiesListViewItemPainter::showAccountName() const
{
	if (Index.parent().isValid())
		return true;

	return Option.state & QStyle::State_MouseOver && Configuration.showAccountName();
}

bool BuddiesListViewItemPainter::showDescription() const
{
	if (!Configuration.showDescription())
		return false;

	QString description = Index.data(DescriptionRole).toString();
	return !description.isEmpty();
}

void BuddiesListViewItemPainter::computeIconRect()
{
	IconRect = QRect(0, 0, 0, 0);

	QPixmap icon = buddyIcon();
	if (icon.isNull())
		return;

	QPoint topLeft = ItemRect.topLeft();
	IconRect = icon.rect();

	if (!Configuration.alignTop())
		topLeft.setY(topLeft.y() + (ItemRect.height() - icon.height()) / 2);
	else
		topLeft.setY(topLeft.y() + (fontMetrics().lineSpacing() + 3 - icon.height()) / 2);

	IconRect.moveTo(topLeft);
}

void BuddiesListViewItemPainter::computeMessageIconRect()
{
	MessageIconRect = QRect(0, 0, 0, 0);
	if (!showMessagePixmap())
		return;

	QPixmap icon = Configuration.messagePixmap();

	QPoint topLeft = ItemRect.topLeft();
	MessageIconRect = icon.rect();

	if (!Configuration.alignTop())
		topLeft.setY(topLeft.y() + (ItemRect.height() - icon.height()) / 2);
	else
		topLeft.setY(topLeft.y() + (fontMetrics().lineSpacing() + 3 - icon.height()) / 2);

	if (!IconRect.isEmpty())
		topLeft.setX(IconRect.x() + VFrameMargin);

	MessageIconRect.moveTo(topLeft);
}

void BuddiesListViewItemPainter::computeAvatarRect()
{
	AvatarRect = QRect(0, 0, 0, 0);
	if (!Configuration.showAvatars())
		return;

	AvatarRect.setWidth(Configuration.defaultAvatarSize().width() + 2 * HFrameMargin);

	if (!buddyAvatar().isNull())
		AvatarRect.setHeight(Configuration.defaultAvatarSize().height() + 2 * VFrameMargin);
	else
		AvatarRect.setHeight(1); // just a placeholder

	AvatarRect.moveRight(ItemRect.right());
	AvatarRect.moveTop(ItemRect.top());
}

QString BuddiesListViewItemPainter::getAccountName()
{
	Account account = qvariant_cast<Account>(Index.data(AccountRole));
	return account.accountIdentity().name();
}

QString BuddiesListViewItemPainter::getName()
{
	return Index.data(Qt::DisplayRole).toString();
}

QTextDocument * BuddiesListViewItemPainter::createDescriptionDocument(const QString &text, int width, QColor color) const
{
	QString description = Qt::escape(text);
	description.replace("\n", Configuration.showMultiLineDescription() ? "<br/>" : " " );

	QTextDocument *doc = new QTextDocument();

	doc->setDefaultFont(Configuration.descriptionFont());
	if (Configuration.descriptionColor().isValid())
		doc->setDefaultStyleSheet(QString("* { color: %1; }").arg(color.name()));

	doc->setHtml(QString("<span>%1</span>").arg(description));

	QTextOption opt = doc->defaultTextOption();
	opt.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
	doc->setDefaultTextOption(opt);

	QTextFrameFormat frameFormat = doc->rootFrame()->frameFormat();
	frameFormat.setMargin(0);
	doc->rootFrame()->setFrameFormat(frameFormat);

	doc->setTextWidth(width);
	return doc;
}

QTextDocument * BuddiesListViewItemPainter::getDescriptionDocument(int width)
{
	if (DescriptionDocument)
		return DescriptionDocument;

	QColor textcolor = Option.palette.color(QPalette::Normal, Option.state & QStyle::State_Selected
			? QPalette::HighlightedText
			: QPalette::Text);

	DescriptionDocument = createDescriptionDocument(Index.data(DescriptionRole).toString(), width,
			Option.state & QStyle::State_Selected
					? textcolor
					: Configuration.descriptionColor());

	return DescriptionDocument;
}

void BuddiesListViewItemPainter::computeAccountNameRect()
{
	AccountNameRect = QRect(0, 0, 0, 0);
	if (!showAccountName())
		return;

	QString accountDisplay = getAccountName();

	int accountDisplayWidth = DescriptionFontMetrics.width(accountDisplay);

	AccountNameRect.setWidth(accountDisplayWidth);
	AccountNameRect.setHeight(DescriptionFontMetrics.height());

	AccountNameRect.moveRight(ItemRect.right() - AvatarRect.width() - HFrameMargin);
	AccountNameRect.moveTop(ItemRect.top());
}

void BuddiesListViewItemPainter::computeNameRect()
{
	NameRect = QRect(0, 0, 0, 0);

	int left = qMax(IconRect.right(), MessageIconRect.right());
	if (!IconRect.isEmpty() || !MessageIconRect.isEmpty())
		left += HFrameMargin;
	else
		left = ItemRect.left();

	int right;
	if (!AccountNameRect.isEmpty())
		right = AccountNameRect.left() - HFrameMargin;
	else if (!AvatarRect.isEmpty())
		right = AvatarRect.left() - HFrameMargin;
	else
		right = ItemRect.right() - HFrameMargin;

	NameRect.moveTop(ItemRect.top());
	NameRect.setLeft(left);
	NameRect.setRight(right);
	NameRect.setHeight(fontMetrics().height());
}

void BuddiesListViewItemPainter::computeDescriptionRect()
{
	DescriptionRect = QRect(0, 0, 0, 0);

	if (!showDescription())
		return;

	DescriptionRect.setTop(NameRect.bottom() + VFrameMargin);
	DescriptionRect.setLeft(NameRect.left());
	DescriptionRect.setRight(AvatarRect.left() - HFrameMargin);
	DescriptionRect.setHeight((int)getDescriptionDocument(DescriptionRect.width())->size().height());
}

void BuddiesListViewItemPainter::computeLayout()
{
	computeIconRect();
	computeMessageIconRect();
	computeAvatarRect();
	computeAccountNameRect();
	computeNameRect();
	computeDescriptionRect();
}

QPixmap BuddiesListViewItemPainter::buddyAvatar() const
{
	QVariant avatar = Index.data(AvatarRole);
	if (!avatar.canConvert<QPixmap>())
		return QPixmap();

	return avatar.value<QPixmap>();
}

QPixmap BuddiesListViewItemPainter::buddyIcon() const
{
	return qvariant_cast<QPixmap>(Index.data(Qt::DecorationRole));
}

const QFontMetrics & BuddiesListViewItemPainter::fontMetrics()
{
	if (useBold())
		return BoldFontMetrics;
	else
		return FontMetrics;
}

int BuddiesListViewItemPainter::itemIndentation()
{
	int level = 0;

	if (Widget->rootIsDecorated())
		level++;

	if (Index.parent().isValid())
		level++;

	return level * Widget->indentation();
}

int BuddiesListViewItemPainter::height()
{
	if (!Widget)
		return 0;

	ItemRect = QRect(0, 0, 0, 20);

	QHeaderView *header = Widget->header();
	if (header)
		ItemRect.setWidth(header->sectionSize(0) - itemIndentation());

	ItemRect.adjust(HFrameMargin, VFrameMargin, -HFrameMargin, -VFrameMargin);

	computeLayout();

	QRect wholeRect = IconRect;
	wholeRect |= MessageIconRect;
	wholeRect |= AvatarRect;
	wholeRect |= AccountNameRect;
	wholeRect |= NameRect;
	wholeRect |= DescriptionRect;

	return wholeRect.height() + 2 * VFrameMargin;
}

void BuddiesListViewItemPainter::paintDebugRect(QPainter *painter, QRect rect, QColor color) const
{
	painter->save();
	painter->setPen(color);
	painter->drawRect(rect);
	painter->restore();
}

void BuddiesListViewItemPainter::paintIcon(QPainter *painter)
{
	QPixmap icon = buddyIcon();
	if (icon.isNull())
		return;

	painter->drawPixmap(IconRect, icon);
}

void BuddiesListViewItemPainter::paintMessageIcon(QPainter *painter)
{
	if (!showMessagePixmap())
		return;

	painter->drawPixmap(MessageIconRect, Configuration.messagePixmap());
}

void BuddiesListViewItemPainter::paintAvatar(QPainter *painter)
{
	QRect rect = AvatarRect.adjusted(VFrameMargin, HFrameMargin, -VFrameMargin, -HFrameMargin);

	BuddiesListViewAvatarPainter avatarPainter(Configuration, Option, rect, Index);
	avatarPainter.paint(painter);
}

void BuddiesListViewItemPainter::paintAccountName(QPainter *painter)
{
	if (!showAccountName())
		return;

	painter->setFont(Configuration.descriptionFont());
	painter->drawText(AccountNameRect, getAccountName());
}

void BuddiesListViewItemPainter::paintName(QPainter *painter)
{
	if (useBold())
		painter->setFont(Configuration.boldFont());
	else
		painter->setFont(Configuration.font());

	painter->drawText(NameRect, fontMetrics().elidedText(getName(), Qt::ElideRight, NameRect.width()));
}

void BuddiesListViewItemPainter::paintDescription(QPainter *painter)
{
	if (!showDescription())
		return;
  
	painter->setFont(Configuration.descriptionFont());
	painter->save();
	painter->translate(DescriptionRect.topLeft());
	getDescriptionDocument(DescriptionRect.width())->drawContents(painter);
	painter->restore();
}

void BuddiesListViewItemPainter::paint(QPainter *painter)
{
	ItemRect = Option.rect;
	ItemRect.adjust(HFrameMargin, VFrameMargin, -HFrameMargin, -VFrameMargin);

	computeLayout();

	if (Option.state & QStyle::State_Selected)
		painter->setPen(Option.palette.color(QPalette::Normal, QPalette::HighlightedText));
	else
		painter->setPen(Configuration.fontColor());
	
	paintIcon(painter);
	paintMessageIcon(painter);
	paintAvatar(painter);
	paintAccountName(painter);
	paintName(painter);
	paintDescription(painter);

	/*
	paintDebugRect(painter, ItemRect, QColor(255, 0, 0));
	paintDebugRect(painter, IconRect, QColor(0, 255, 0));
	paintDebugRect(painter, MessageIconRect, QColor(255, 255, 0));
	paintDebugRect(painter, AvatarRect, QColor(0, 0, 255));
	paintDebugRect(painter, AccountNameRect, QColor(255, 0, 255));
	paintDebugRect(painter, NameRect, QColor(0, 255, 255));
	paintDebugRect(painter, DescriptionRect, QColor(0, 0, 0));
	*/
}