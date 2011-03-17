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


#include "printrangpicker.h"
#include "ui_printrangpicker.h"

#include <QImage>
#include "rugitem.h"
#include "threadpattern.h"


PrintRangPicker::PrintRangPicker(RugItem * rug, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PrintRangPicker)
{
    ui->setupUi(this);

    ui->rugName->setText(rug->getRugName());
    ui->patternSize->setText(QString::number(rug->getThreadPattern()->getImage().width()));
    ui->startWidth->setValue(rug->getRugSize().width());
    ui->endWidth->setValue(rug->getRugSize().width());
    ui->step->setValue(0);
    ui->rugHeight->setValue(rug->getRugSize().height());
}

PrintRangPicker::~PrintRangPicker()
{
    delete ui;
}

int PrintRangPicker::getProp(RProp rp)
{
	switch(rp)
	{
	case RangeStart: return ui->startWidth->value();
	case RangeStop : return ui->endWidth->value();
	case RangeStep : return ui->step->value();
	case RangeRugHeight : return ui->rugHeight->value();
	default:break;
	}
	return 0;
}

