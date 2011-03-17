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


#include "patternimporter.h"

#include <QProgressDialog>
#include <QDebug>
#include <QInputDialog>

PatternImporter::PatternImporter(const QString& fn)
{
	image = QImage(fn);
	if(!image.isNull())
	{
		m_ts = QInputDialog::getInt(0, "foo", "threshold", 2, 1, 100);
		findPattern();
	}
}

void PatternImporter::findPattern()
{
	if(image.height() < 20)
		return;
	QImage l1(image.copy(0,0,image.width(),1));

	bool hasPattern(false);
	for(int y(10); y < image.height(); ++y)
	{
		QImage l2(image.copy(0,y,image.width(),1));
		if(l1 == l2)
		{
			hasPattern = true;
			break;
		}
	}
	QList<unsigned int> allRug;
	bool LTR(true);
	for(int y(0); y < image.height() ; ++y)
	{
		if(LTR)
		{
			for(int x(0); x < image.width(); ++x)
			{
				allRug << image.pixel(x,y);
			}
			LTR = false;
		}
		else
		{
			for(int x(image.width() - 1); x >= 0; --x)
			{
				allRug << image.pixel(x,y);
			}
			LTR = true;
		}
	}
	int limit(allRug.count() / 2);
	QProgressDialog pd("Analyzing Image", "Cancel", 1, limit);
	pd.setWindowModality(Qt::WindowModal);
	pd.setMinimumDuration(0);
	pd.setValue(1);
	int pc(0);
	for(int i(2); i < limit; ++i)
	{
		if(pd.wasCanceled())
			return;
		QList<unsigned int> p(allRug.mid(0, i));
		if(checkPattern(allRug, p))
		{
			pc = i;
			break;
		}
		pd.setValue(i);
	}
	if(pc > 0)
	{
		pattern = QImage(pc*2, 1, QImage::Format_ARGB32);
		int p(0);
		for(int i(0); i < pc; ++i)
		{
			pattern.setPixel(p,0, allRug.at(i));
			if(i < (pc + 1))
			{
				pattern.setPixel(p + 1,0, intermediateColor( allRug.at(i), allRug.at(i+1)));
				p += 2;
			}
			else
				++p;
		}
	}

}


bool PatternImporter::checkPattern(const QList<unsigned int> &ar, const QList<unsigned int> &p)
{
	int limit(ar.count() - p.count());

	int hint(0);
	for(int i(p.count()); i < limit; i += p.count())
	{
		QList<unsigned int> tc(ar.mid(i, p.count()));
		if(!compareColorList(p,tc))
		{
			//qDebug()<<"CP"<<p.count()<<hint;
			return false;
		}
		++hint;
	}
	return true;
}

bool PatternImporter::compareColorList(const QList<unsigned int> &a, const QList<unsigned int> &b)
{
	if(a.count() != b.count())
		return false;
	int c(a.count());
	for(int i(0); i < c; ++i)
	{
		if(!compareColor(a.at(i), b.at(i), m_ts))
			return false;
	}
	return true;
}

bool PatternImporter::compareColor(unsigned int a, unsigned b, int treshold)
{
	QRgb ca(a);
	QRgb cb(b);
	int ra(qRed(ca));
	int ga(qGreen(ca));
	int ba(qBlue(ca));
	int rb(qRed(cb));
	int gb(qGreen(cb));
	int bb(qBlue(cb));
	if(qAbs(ra - rb) > treshold)
		return false;
	else if(qAbs(ga - gb) > treshold)
		return false;
	else if(qAbs(ba - bb) > treshold)
		return false;

	return true;
}

unsigned int PatternImporter::intermediateColor(unsigned int a, unsigned b)
{
	QRgb ca(a);
	QRgb cb(b);
	int ra(qRed(ca));
	int ga(qGreen(ca));
	int ba(qBlue(ca));
	int rb(qRed(cb));
	int gb(qGreen(cb));
	int bb(qBlue(cb));

	int rr((ra + rb) / 2);
	int gr((ga + gb) / 2);
	int br((ba + bb) / 2);

	return QColor(rr,gr,br).rgb();
}

QImage PatternImporter::getPattern() const
{
	return pattern;
}
