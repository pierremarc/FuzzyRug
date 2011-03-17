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


#ifndef RUGPRINTER_H
#define RUGPRINTER_H

#include <QObject>
#include <QRectF>
#include <QSize>

class RugItem;
class QPainter;
class QPrintDialog;
class QPrinter;
class QProgressDialog;

class RugPrinter : public QObject
{
	Q_OBJECT

	static RugPrinter * instance;
	RugPrinter();

	QPrinter * printer;
	QPrintDialog * pDialog;
	RugItem * currentRug;
	QProgressDialog * progress;

	void printThread(QPainter& p);
	void printRug(QPainter& p, QSize rs);

	int rStart;
	int rStop;
	int rStep;
	int rHeight;

public:
	static void print(RugItem * rug);

private slots:
	void pAccept();
};

#endif // RUGPRINTER_H
