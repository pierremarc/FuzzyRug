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


#ifndef FUZZYLOADER_H
#define FUZZYLOADER_H

#include <QObject>
#include <QString>
#include <QMap>

class RugItem;

class FuzzyLoader : public QObject
{
	Q_OBJECT

	const QString filePath;

public:
	FuzzyLoader(const QString& fn);
	void load(QMap<QString, RugItem*>* rugs);

signals:
	void addPattern(QString);
	void addRug(RugItem*);

};

#endif // FUZZYLOADER_H
