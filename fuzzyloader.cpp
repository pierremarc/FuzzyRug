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


#include "fuzzyloader.h"

#include <QDir>
#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamAttributes>
#include <QDebug>
#include <QLocale>

#include "threadpatterncollection.h"
#include "threadpattern.h"
#include "rugitem.h"


FuzzyLoader::FuzzyLoader(const QString& fn):
		filePath(fn)
{
}


void FuzzyLoader::load(QMap<QString, RugItem *> *rugs)
{
	QLocale sLoc;
	QLocale::setDefault(QLocale::C);
	foreach(const QString& k, rugs->keys())
	{
		delete rugs->value(k);
	}
	rugs->clear();

	ThreadPatternCollection * tpc(ThreadPatternCollection::getInstance());
	tpc->clear();

	QFile f(filePath);
	f.open(QIODevice::ReadOnly);

	QXmlStreamReader reader(&f);
	if (reader.readNextStartElement())
	{
		if (reader.name() != "fuzzysession" )
			return;
	}
	else
		return;

	while (reader.readNextStartElement())
	{
		qDebug()<<"RN"<<reader.name();
		if (reader.name() == "patternpath")
		{
			QString path(reader.readElementText());
			emit addPattern(path);
			qDebug()<<"PPATH"<<path;
		}
		else if (reader.name() == "rug")
		{
			qDebug()<<"RUG";
			QXmlStreamAttributes attr(reader.attributes());
			RugItem * ri(new RugItem(attr.value("name").toString()));
			QSizeF rs;
			QString hstr,wstr,xstr,ystr,thstr,psstr;
			hstr = attr.value("height").toString();
			wstr = attr.value("width").toString();
			xstr = attr.value("xpos").toString();
			ystr = attr.value("ypos").toString();
			thstr = attr.value("threadheight").toString();
			psstr = attr.value("patternsize").toString();
			rs.setHeight(hstr.toDouble());
			rs.setWidth(wstr.toDouble());
			ri->setRugSize(rs);
			ri->setPos(xstr.toDouble(), ystr.toDouble());
			ri->setThreadHeight(thstr.toDouble());
			ri->setPatternSize(psstr.toDouble());

			while(reader.readNextStartElement())
			{
				if(reader.name() == "pattern")
				{
					QString pattern(reader.readElementText());
					ri->setThreadPattern(tpc->value(pattern));
				}
				else
					reader.skipCurrentElement();
			}

			emit addRug(ri);
		}
		else
			reader.skipCurrentElement();
	}
	QLocale::setDefault(sLoc);

}
