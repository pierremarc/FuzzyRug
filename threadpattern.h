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


#ifndef THREADPATTERN_H
#define THREADPATTERN_H

#include <QImage>

class ThreadPattern
{
	const QString filePath;
	QString name;
	QImage image;

	static ThreadPattern * defaultPattern;

public:
	ThreadPattern(const QString& fileP);
	static ThreadPattern * DefaultPattern();

	void setImage(QImage i){image = i;}
	QImage getImage() const {return image;}

	QString path() const{return filePath;}
	QString getName()const{return name;}
};

#endif // THREADPATTERN_H
