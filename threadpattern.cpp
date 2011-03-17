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


#include "threadpattern.h"
#include <QColor>
#include <QFileInfo>

ThreadPattern * ThreadPattern::defaultPattern = 0;

ThreadPattern::ThreadPattern(const QString& fileP)
	:filePath(fileP)
{
	QFileInfo i(filePath);
	name = i.baseName();
}

ThreadPattern * ThreadPattern::DefaultPattern()
{
	if(defaultPattern == 0)
	{
		QImage i(100,1, QImage::Format_ARGB32);
		i.fill(QColor(Qt::lightGray).rgb());
		defaultPattern = new ThreadPattern(QString("/I/AM/THE/DEFAULT_PATTERN"));
		defaultPattern->setImage(i);
	}
	return defaultPattern;
}
