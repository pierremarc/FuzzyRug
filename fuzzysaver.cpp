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


#include "fuzzysaver.h"

#include <QDir>
#include <QFile>
#include <QXmlStreamWriter>

#include "threadpatterncollection.h"
#include "threadpattern.h"
#include "rugitem.h"

FuzzySaver::FuzzySaver(const QString& dirPath):
		m_dirPath(dirPath)
{
//	if(!QDir::exists(m_dirPath))
//	{
//		QDir::mkpath(m_dirPath);
//	}
//	else
//	{
//		QDir d(m_dirPath);
//		foreach(QString f, d.entryList(QDir::NoDotAndDotDot))
//		{
//			QFile::remove(d.absoluteFilePath(f));
//		}
//	}

}


void FuzzySaver::save(const QMap<QString, RugItem *> &rugs)
{
	if(QFile::exists(m_dirPath))
		QFile::remove(m_dirPath);
	QFile f(m_dirPath);
	f.open(QIODevice::WriteOnly);

	QXmlStreamWriter stream(&f);
	stream.setAutoFormatting(true);
	stream.writeStartDocument();
	stream.writeStartElement("fuzzysession");
	ThreadPatternCollection * tpc(ThreadPatternCollection::getInstance());
	QString dpath(ThreadPattern::DefaultPattern()->path());
	// First save patterns
	foreach(const QString& k, tpc->getCollection().keys())
	{
		QString path(tpc->value(k)->path());
		if(dpath != path)
			stream.writeTextElement("patternpath", path);
	}

	// Now rugs!
	foreach(const QString& k, rugs.keys())
	{
		RugItem * rug(rugs.value(k));
		if(rug->getThreadPattern()->path() != ThreadPattern::DefaultPattern()->path())
		{
			stream.writeStartElement("rug");

			stream.writeAttribute("name", rug->getRugName());
			stream.writeAttribute("xpos", QString::number(rug->x()));
			stream.writeAttribute("ypos", QString::number(rug->y()));
			stream.writeAttribute("width", QString::number(rug->getRugSize().width()));
			stream.writeAttribute("height", QString::number(rug->getRugSize().height()));
			stream.writeAttribute("threadheight", QString::number(rug->getThreadHeight()));

			stream.writeTextElement("pattern", rug->getThreadPattern()->getName());

			stream.writeEndElement(); // rug
		}
	}

	stream.writeEndElement(); // fuzzysession
	stream.writeEndDocument();
}
