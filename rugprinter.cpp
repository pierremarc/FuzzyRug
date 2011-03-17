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


#include "rugprinter.h"

#include "rugitem.h"
#include "threadpattern.h"
#include "printrangpicker.h"
#include "mainwindow.h"

#include <QPainter>
#include <QPrintDialog>
#include <QPrinter>
#include <QImage>
#include <QPen>
#include <QBrush>
#include <QDebug>
#include <QProgressDialog>

RugPrinter * RugPrinter::instance = 0;

RugPrinter::RugPrinter()
{
        printer = new QPrinter(QPrinter::HighResolution);
        pDialog = 0;
}

void RugPrinter::print(RugItem *rug)
{
	if(instance == 0)
		instance = new RugPrinter;
	RugPrinter * rp(instance);

        if(rp->pDialog != 0)
            delete rp->pDialog;
        rp->pDialog = new QPrintDialog(rp->printer, MainWindow::MW());
	rp->currentRug = rug;
	PrintRangPicker prp(rug);
	prp.exec();

	if(prp.result() == QDialog::Rejected)
		return;

	rp->rStart = prp.getProp(PrintRangPicker::RangeStart);
	rp->rStop = prp.getProp(PrintRangPicker::RangeStop);
	rp->rStep = prp.getProp(PrintRangPicker::RangeStep);
	rp->rHeight = prp.getProp(PrintRangPicker::RangeRugHeight);

	rp->pDialog->open(rp, SLOT(pAccept()));
}


void RugPrinter::pAccept()
{
	QPainter painter(printer);


	// estimate iterations
	int iter(0);
	double paperSurface((printer->pageRect(QPrinter::Millimeter).width()) * (printer->pageRect(QPrinter::Millimeter).height() / 10));
	iter += qRound((double(currentRug->getThreadPattern()->getImage().width()) * 1.5) / paperSurface);
	if( rStep != 0 && rStop != rStart)
	{
		for(int bs(rStart); bs <= rStop; bs += rStep)
		{
			++iter;
		}
	}
	else
		++iter;
	QProgressDialog pd(QString("Printing %1").arg(currentRug->getRugName()),"Cancel",0,iter);
	progress = &pd;
	pd.setWindowModality(Qt::WindowModal);
	pd.setMinimumDuration(0);
	pd.setValue(0);

	printThread(painter);
	if(progress->wasCanceled())
		return;
	printer->newPage();

	if( rStep != 0 && rStop != rStart)
	{
		for(int bs(rStart); bs <= rStop; bs += rStep)
		{
			if(progress->wasCanceled())
				return;
			QSize s(bs, rHeight);
			printRug(painter, s);
			printer->newPage();
			progress->setValue(progress->value() + 1);
		}
	}
	else
	{
		QSize s(rStart, rHeight);
		printRug(painter, s);
		progress->setValue(progress->value() + 1);
	}

}


void RugPrinter::printThread(QPainter &p)
{
	QImage it(currentRug->getThreadPattern()->getImage());

	QRectF pr_dp(printer->pageRect(QPrinter::DevicePixel));
	QRectF pr_mm(printer->pageRect(QPrinter::Millimeter));
	double rr(pr_dp.width() / pr_mm.width());
	double tcm(10.0 * rr);

	qDebug()<<"DP"<<pr_dp;
	qDebug()<<"MM"<<pr_mm;
	qDebug()<<"RR"<<rr<<"TCM"<<tcm;

	QPen tPen;
	p.save();
	p.translate(-pr_dp.left(), -pr_dp.top());

	double x(pr_dp.left());
	double y(pr_dp.top());
	double lastTY(y);
	QRectF ires;
	p.setPen(tPen);
	p.drawText(QRectF(x,y,pr_dp.width(),pr_dp.height()),Qt::AlignLeft | Qt::AlignTop, QString::number(0),&ires);
	y = ires.bottom();
	for(int ix(0); ix < it.width(); ++ix)
	{
		if(progress->wasCanceled())
			return;
		if(x + tcm > pr_dp.right())
		{
			QRectF res;
			p.setPen(tPen);
			p.drawText(QRectF(pr_dp.left(),lastTY,pr_dp.width(),pr_dp.height()),Qt::AlignRight | Qt::AlignTop, QString::number(ix),&res);
			x = pr_dp.left();
			if(y + (2*tcm) > pr_dp.bottom())
			{
				printer->newPage();
				y = pr_dp.top();
				progress->setValue(progress->value() + 1);
			}
			else
			{
				y += tcm * 1.1;
			}
			p.setPen(tPen);
			p.drawText(QRectF(x,y,pr_dp.width(),pr_dp.height()),Qt::AlignLeft | Qt::AlignTop, QString::number(ix),&res);
			lastTY = y;
			y = res.bottom();
		}
		p.setPen(Qt::NoPen);
		p.setBrush(QColor(it.pixel(ix, 0)));
		p.drawRect(x,y,tcm,tcm);
		x+=tcm;
	}
	p.setPen(tPen);
	ires = p.boundingRect(QRectF(pr_dp.left(),lastTY,x - pr_dp.left(),pr_dp.height()),Qt::AlignRight | Qt::AlignTop, QString::number(it.width()));
	p.setBrush(Qt::white);
	p.drawRect(ires);
	p.drawText(QRectF(pr_dp.left(),lastTY,x - pr_dp.left(),pr_dp.height()),Qt::AlignRight | Qt::AlignTop, QString::number(it.width()));

	p.restore();
}



void RugPrinter::printRug(QPainter &p, QSize rs)
{
	QImage ri(currentRug->getBuffer(rs));

	QRect pr(printer->pageRect(QPrinter::DevicePixel).toRect());
	int maxW(pr.width());
	int w(ri.width() * maxW / rStop);
	int h(ri.height() * maxW / rStop);
	QRectF sourceRect(0, 0, ri.width(), ri.height());
	QRectF targetRect(0, 0, w, h);
	p.drawImage(targetRect, ri, sourceRect);

	p.save();
	p.setPen(Qt::NoPen);
	p.setBrush(QBrush(Qt::white));
	double quart(double(printer->paperSize(QPrinter::DevicePixel).height()) * 0.25);
	p.drawRect(0, 3* quart,pr.right(),pr.bottom());

	QString note(QString("width: %1            height: %2            thread height: %3")
		     .arg(rs.width())
		     .arg(currentRug->getRugSize().height())
		     .arg(currentRug->getThreadHeight()));
	p.setPen(QPen());
	p.drawText(40, (3* quart) + (quart / 2), note);

	p.restore();
}






