/***************************************************************************
 *   Copyright (C) 2011 by Pierre Marchand   *
 *   pierre@oep-h.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#include "rugitem.h"
#include "threadpattern.h"

#include <QPainter>
#include <QPen>
#include <QDebug>
#include <QCursor>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <QGraphicsView>

#include <cmath>

#define RUGITEM_MARGIN 3

RugItem::RugItem(const QString& name) :
		rugName(name),
		threadPattern(ThreadPattern::DefaultPattern()),
		threadHeight(1),
		patternSize(ThreadPattern::DefaultPattern()->getImage().widthMM()),
		rugSize(QSizeF(200,200))
{
	setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable );
	setCursor(Qt::OpenHandCursor);
	setAcceptHoverEvents(true);
	validBuffer = false;
	rugHandle = HandleNone;
}

double RugItem::nextStep(double base)
{
	double ret(0);
	double step(2*threadHeight);
	if((base + step) >= patternSize)
	{
		ret = (base + step) - patternSize;
	}
	else
		ret = base + step;

	return ret;

}

QColor RugItem::colorAt(double pos)
{
	QImage img(threadPattern->getImage());
	// TODO build a color that would take in account the thickness of the thread such as if it goes beyond one "pixel" we would return a mix of colors
	return QColor(img.pixel(floor(pos * (double(img.width()) / patternSize) ),0));
}

void RugItem::fillBuffer()
{
	QImage img(threadPattern->getImage());
	QPen pen;
	pen.setWidth(1);
	double bottom(RUGITEM_MARGIN + rugSize.height());
	double right(RUGITEM_MARGIN + rugSize.width());
	buffer = QImage(boundingRect().toRect().size(), QImage::Format_ARGB32);
	buffer.fill(QColor(Qt::white).rgb());
	QPainter pp(&buffer);
	double tc(0);
	bool LTR(true);
	for(int y(RUGITEM_MARGIN ); y < bottom ; ++y)
	{
		if(LTR)
		{
			for(int x(RUGITEM_MARGIN ); x < right; ++x)
			{
				pen.setColor(colorAt(tc));
				pp.setPen(pen);
				pp.drawPoint(x,y);
				tc = nextStep(tc);
			}
			LTR = false;
		}
		else
		{
			for(int x(right - 1); x >= RUGITEM_MARGIN; --x)
			{
				pen.setColor(colorAt(tc));
				pp.setPen(pen);
				pp.drawPoint(x,y);
				tc = nextStep(tc);
			}
			LTR = true;
		}
	}
}

void RugItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	QPen pen;
	pen.setWidth(1);
	pen.setCosmetic(true);

	if(!validBuffer)
	{
		fillBuffer();
		validBuffer = true;
	}
	painter->drawImage(QPoint(0,0), buffer);
	pen.setColor(Qt::black);
	if(isSelected())
		pen.setColor(Qt::blue);
	painter->setPen(pen);
	painter->setBrush(Qt::NoBrush);
	painter->drawRect(boundingRect());
	if(isSelected())
	{
		pen.setCosmetic(false);
		double ss(scene()->views().first()->transform().inverted().m11());
		painter->setRenderHint(QPainter::Antialiasing, true);
		double mv((rugSize.height() + (2*RUGITEM_MARGIN)) / 2.0);
		double mh((rugSize.width() + (2*RUGITEM_MARGIN)) / 2.0);
		double scaledM(double(RUGITEM_MARGIN) * ss);
		double scaledP(double(pen.widthF() / 2.0) * ss);
		pen.setWidthF(scaledP * 2.0);
		painter->setPen(pen);
//		qDebug()<<ss<<scaledM<<scaledP;
//		painter->setBrush(pen.color());
		// bottom
		QRectF ba(mh - scaledM ,
		       boundingRect().height() + scaledP - scaledM,
		       2*scaledM, 2*scaledM);
		painter->drawArc(ba, 0 ,2 *90 *16);
		// right
		QRectF ra(boundingRect().width() + scaledP - scaledM,
			  mv - scaledM,
			  2*scaledM, 2*scaledM);
		painter->drawArc(ra, 90 *16 , 2 *90 *16);
//		pen.setColor(Qt::red);
//		painter->setPen(pen);
//		painter->drawRect(ra);
	}
}

QRectF RugItem::boundingRect() const
{
	double tm(2 * RUGITEM_MARGIN);
	return QRectF(0, 0, double(tm + rugSize.width()), double(tm + rugSize.height()));
}

void RugItem::setThreadHeight(double t)
{
	if(t != threadHeight)
		validBuffer = false;
	threadHeight = t;
	update();
}

void RugItem::setPatternSize(double s)
{
	if(s != patternSize)
		validBuffer = false;
	patternSize = s;
	update();
}

void RugItem::setThreadPattern(ThreadPattern *t, bool updatePSize)
{
	if(t != threadPattern)
		validBuffer = false;
	threadPattern = t;
	if(updatePSize)
		patternSize = t->getImage().widthMM();
	update();
}

void RugItem::setRugSize(QSizeF s)
{
	if(s != rugSize)
	{
		validBuffer = false;
		prepareGeometryChange();
		rugSize = s;
	}
}

void RugItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	if(rugHandle == HandleNone)
		QGraphicsObject::mouseMoveEvent(event);
	else
	{
		validBuffer = false;
		prepareGeometryChange();
		if(rugHandle == HandleRight)
		{
			double spnx(event->scenePos().x());
			double spox( mapToScene( boundingRect().topRight()).x() );
			rugSize.rwidth() += spnx - spox;
		}
		else if(rugHandle == HandleBottom)
		{
			double spny(event->scenePos().y());
			double spoy( mapToScene( boundingRect().bottomLeft()).y() );
			rugSize.rheight() += spny - spoy;
		}
		emit changeSizeInteractive(rugSize);
	}
}

void RugItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	isMoving = true;
	if(rugHandle == HandleNone)
		setCursor(Qt::ClosedHandCursor);
	else if(rugHandle == HandleRight)
		setCursor(Qt::SizeHorCursor);
	else if(rugHandle == HandleBottom)
		setCursor(Qt::SizeVerCursor);
	QGraphicsObject::mousePressEvent(event);
}


void RugItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	emit rugSelected(rugName);
	isMoving = false;
	rugHandle = HandleNone;
	setCursor(Qt::OpenHandCursor);
	QGraphicsObject::mouseReleaseEvent(event);
}

void RugItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
	if(!isSelected())
		return;
	// check if in handles zones
	double ss(scene()->views().first()->transform().inverted().m11());
	double scaledM(double(RUGITEM_MARGIN) * ss);
	double mv((rugSize.height() + (2*RUGITEM_MARGIN)) / 2.0);
	double mh((rugSize.width() + (2*RUGITEM_MARGIN)) / 2.0);
	// bottom handle
	QRectF bh(mh - (2*scaledM),
	       boundingRect().height() - (2*scaledM),
	       4*scaledM, 2*scaledM);
	// right handle
	QRectF rh(boundingRect().width() - (2*scaledM),
		  mv - (2*scaledM),
		  2 * scaledM, 4*scaledM);
//	qDebug()<<rh<<event->pos();
	if(bh.contains(event->pos()))
	{
		rugHandle = HandleBottom;
		setCursor(Qt::SizeVerCursor);
	}
	else if(rh.contains(event->pos()))
	{
		rugHandle = HandleRight;
		setCursor(Qt::SizeHorCursor);
	}
	else
	{
		rugHandle = HandleNone;
		setCursor(Qt::OpenHandCursor);
	}

}

void RugItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
	isMoving = false;
	setCursor(Qt::OpenHandCursor);
}

QImage RugItem::getBuffer(QSizeF s)
{
	QSizeF storeS(rugSize);
	rugSize = s;
	fillBuffer();
	rugSize = storeS;
	validBuffer = false;
	return buffer;
}






