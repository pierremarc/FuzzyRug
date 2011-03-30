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


#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "threadpatterncollection.h"
#include "rugitem.h"
#include "rugprinter.h"

#include "fuzzysaver.h"
#include "fuzzyloader.h"

#include "patternimporter.h"

#include <QImage>
#include <QInputDialog>
#include <QFileDialog>
#include <QFileInfo>
#include <QTransform>
#include <QMessageBox>


MainWindow * MainWindow::MWInstance = 0;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    currentRug(0)
{
	MWInstance = this;
    ui->setupUi(this);
    setWindowTitle("FuzzyRug");
    rugScene = new QGraphicsScene(0,0, 5000, 5000);

    ui->actionNew->setShortcut(QKeySequence::New);
    ui->actionOpen->setShortcut(QKeySequence::Open);
    ui->actionSave->setShortcut(QKeySequence::Save);
    ui->actionSave_As->setShortcut(QKeySequence::SaveAs);
    ui->actionPrint->setShortcut(QKeySequence::Print);
    ui->actionQuit->setShortcut(QKeySequence::Quit);

    ui->rugHeight->setRange(1.0, 100000.0);
    ui->rugWidth->setRange(1.0, 100000.0);
    ui->patternSize->setRange(1.0, 100000.0);
    ui->threadHeight->setRange(1.0, 10000.0);
    ui->wp100->setRange(1.0, 100000.0);

    ui->rugView->setScene(rugScene);

    connect(ui->addPattern, SIGNAL(clicked()), this, SLOT(slotAddPattern()));
    connect(ui->importPattern, SIGNAL(clicked()), this, SLOT(slotImportPattern()));
    connect(ui->addRug, SIGNAL(clicked()), this, SLOT(slotAddRug()));

    connect(ui->rugList, SIGNAL(itemSelectionChanged()), this, SLOT(slotChangeCurrentRug()));
    connect(ui->patternList, SIGNAL(itemSelectionChanged()), this, SLOT(slotSelectPattern()));

    connect(ui->rugHeight, SIGNAL(valueChanged(double)), this, SLOT(slotChangeRugHeight(double)));
    connect(ui->rugWidth, SIGNAL(valueChanged(double)), this, SLOT(slotChangeRugWidth(double)));
    connect(ui->threadHeight, SIGNAL(valueChanged(double)), this, SLOT(slotChangeThreadHeight(double)));
    connect(ui->patternSize, SIGNAL(valueChanged(double)), this, SLOT(slotChangePatternSize(double)));
    connect(ui->wp100, SIGNAL(valueChanged(double)), this, SLOT(slotChangeWP100(double)));

    connect(ui->zoom, SIGNAL(valueChanged(int)), this, SLOT(slotZoom(int)));
    connect(ui->resetZoom, SIGNAL(clicked()), this, SLOT(slotResetZoom()));

    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(slotOpen()));
    connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(slotNew()));
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(slotSave()));
    connect(ui->actionSave_As, SIGNAL(triggered()), this, SLOT(slotSaveAs()));
    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionPrint, SIGNAL(triggered()), this, SLOT(slotPrint()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

MainWindow * MainWindow::MW()
{
	return MWInstance;
}

void MainWindow::blockSpinBoxesSignal(bool b)
{
	ui->rugHeight->blockSignals(b);
	ui->rugWidth->blockSignals(b);
	ui->threadHeight->blockSignals(b);
	ui->patternSize->blockSignals(b);
	ui->wp100->blockSignals(b);
}

void MainWindow::updateSpinBoxes()
{
	blockSpinBoxesSignal(true);
	ui->rugHeight->setValue(currentRug->getRugSize().height());
	ui->rugWidth->setValue(currentRug->getRugSize().width());
	ui->threadHeight->setValue(currentRug->getThreadHeight());
	ui->patternSize->setValue(currentRug->getPatternSize());
	ui->wp100->setValue(currentRug->getWP100());
	blockSpinBoxesSignal(false);
}

void MainWindow::addPattern(QString path)
{
	QFileInfo fni(path);
	QString basename(fni.baseName());

	ThreadPatternCollection * tpc(ThreadPatternCollection::getInstance());
	if(tpc->contains(basename))
		return;

	QImage i(path);
	if(i.isNull())
		return;
	ThreadPattern * tp(new ThreadPattern(path));
	tp->setImage(i);
	tpc->insert(basename, tp);

	ui->patternList->addItem(basename);
}

void MainWindow::slotAddPattern()
{
	QString fn(QFileDialog::getOpenFileName(this, QString("Open Pattern")));
	if(fn.isEmpty())
		return;

	addPattern(fn);
}

void MainWindow::slotImportPattern()
{
	QString fn(QFileDialog::getOpenFileName(this, QString("Open Pattern")));
	if(fn.isEmpty())
		return;
	PatternImporter pI(fn);
	if(pI.getPattern().isNull())
	{
		QMessageBox::information(this, "bla", QString("Unable to extract a pattern from:\n %1").arg(fn));
		return;
	}

	QString sn(QFileDialog::getSaveFileName(this, "save pattern"));
	if(sn.isEmpty())
		return;
	if(!sn.endsWith(".png"))
		sn.append(".png");
	pI.getPattern().save(sn);
	addPattern(sn);
}

void MainWindow::addRug(RugItem *ri)
{
	rugs.insert(ri->getRugName(), ri);
//	currentRug = ri;
	ui->rugList->addItem(ri->getRugName());
	rugScene->addItem(ri);

	connect(ri, SIGNAL(rugSelected(QString)), this, SLOT(slotChangeCurrentRug(QString)));
	connect(ri, SIGNAL(changeSizeInteractive(QSizeF)), this, SLOT(slotUpdateSizeFromCanvas(QSizeF)));

}

void MainWindow::slotAddRug()
{
	QString rn(QInputDialog::getText(this, "new rug", "Rug Name:"));
	if(rn.isEmpty())
		return;

	RugItem * ri(new RugItem(rn));
	QSizeF rf(ri->boundingRect().size());
	QPointF pf(ui->rugView->mapToScene(0,0));
	QPointF pf2(ui->rugView->mapToScene(ui->rugView->width(), ui->rugView->height()));
	double w(pf2.x() - pf.x());
	double h(pf2.y() - pf.y());
	double x(((w - (rf.width() )) / 2.0) + pf.x());
	double y(((h - (rf.height() )) / 2.0) + pf.y());
	ri->setPos(x,y);
	addRug(ri);
	ui->rugList->setCurrentRow(ui->rugList->count() - 1);
}

void MainWindow::slotChangeCurrentRug()
{
	QList<QListWidgetItem *> its(ui->rugList->selectedItems());
	if(its.isEmpty())
		return;
	QString rn(its.first()->text());
	slotChangeCurrentRug(rn);
	currentRug->ensureVisible();
}

void MainWindow::slotChangeCurrentRug(const QString& rn)
{
	QList<QListWidgetItem *> its(ui->rugList->findItems(rn, Qt::MatchExactly));
	if(its.isEmpty())
		return;
	foreach(RugItem * i, rugs.values())
	{
		i->setSelected(false);
		i->setZValue(0);
	}
	disconnect(ui->rugList, SIGNAL(itemSelectionChanged()), this, SLOT(slotChangeCurrentRug()));
	ui->rugList->setCurrentItem(its.first());
	connect(ui->rugList, SIGNAL(itemSelectionChanged()), this, SLOT(slotChangeCurrentRug()));
	currentRug = rugs.value(rn);

        disconnect(ui->patternList, SIGNAL(itemSelectionChanged()), this, SLOT(slotSelectPattern()));
        QString pString(currentRug->getThreadPattern()->getName());
        for(int pit(0);pit < ui->patternList->count(); ++pit)
        {
            if(ui->patternList->item(pit)->text() == pString)
            {
                ui->patternList->setCurrentRow(pit);
                break;
            }
        }
        connect(ui->patternList, SIGNAL(itemSelectionChanged()), this, SLOT(slotSelectPattern()));

	currentRug->setSelected(true);
	currentRug->setZValue(1.0);
	updateSpinBoxes();
}

void MainWindow::slotSelectPattern()
{
	QList<QListWidgetItem *> its(ui->patternList->selectedItems());
	if(its.isEmpty())
		return;
	QString pn(its.first()->text());
	ThreadPatternCollection * tpc(ThreadPatternCollection::getInstance());
	ThreadPattern * tp(ThreadPattern::DefaultPattern());
	if(currentRug && tpc->contains(pn))
		tp = tpc->value(pn);
	currentRug->setThreadPattern(tp);
	QImage im(tp->getImage());
	ui->rugWidth->setValue( im.widthMM() / 2.0 );
	ui->patternSize->setValue(im.widthMM());

}

void MainWindow::slotChangeRugHeight(double v)
{
	if(!currentRug)
		return;
	QSizeF s(currentRug->getRugSize());
	s.setHeight(v);
	currentRug->setRugSize(s);
}

void MainWindow::slotChangeRugWidth(double v)
{
	if(!currentRug)
		return;
	QSizeF s(currentRug->getRugSize());
	s.setWidth(v);
	currentRug->setRugSize(s);
}


void MainWindow::slotChangeThreadHeight(double v)
{
	if(!currentRug)
		return;
	currentRug->setThreadHeight(v);
}

void MainWindow::slotChangePatternSize(double v)
{
	if(!currentRug)
		return;
	currentRug->setPatternSize(v);
}

void MainWindow::slotChangeWP100(double v)
{
	if(!currentRug)
		return;
	currentRug->setWP100(v);
}

void MainWindow::slotZoom(int s)
{
	QTransform t;
	t.scale(double(s) / 100.0 ,double(s) / 100.0);
	ui->zoomLabel->setText(QString("%1 \%").arg(s));
	ui->rugView->setTransform(t);
}

void MainWindow::slotResetZoom()
{
	ui->zoom->setValue(100);
}

void MainWindow::slotOpen()
{
	QFileInfo i(currentFile);
	QString fn(QFileDialog::getOpenFileName(this, "bla", i.dir().absolutePath()));
	if(fn.isEmpty())
		return;
	ui->rugList->clear();
	ui->patternList->clear();
	setCurrentFile( fn );
	FuzzyLoader fl(currentFile);
	FuzzyLoader * flp(&fl);
	connect(flp, SIGNAL(addPattern(QString)), this, SLOT(addPattern(QString)));
	connect(flp, SIGNAL(addRug(RugItem*)), this, SLOT(addRug(RugItem*)));
	fl.load(&rugs);
}

void MainWindow::slotNew()
{
	foreach(const QString& k, rugs.keys())
	{
		delete rugs.value(k);
	}
	rugs.clear();
	ThreadPatternCollection * tpc(ThreadPatternCollection::getInstance());
	tpc->clear();
	ui->rugList->clear();
	ui->patternList->clear();
	setCurrentFile(QString());
	currentRug = 0;

}

void MainWindow::slotSave()
{
	if(currentFile.isEmpty())
		slotSaveAs();
	else
	{
		FuzzySaver fs(currentFile);
		fs.save(rugs);
	}
}

void MainWindow::slotSaveAs()
{
	QFileInfo i(currentFile);
	QString fn(QFileDialog::getSaveFileName(this, "bla", i.dir().absolutePath()));
	if(fn.isEmpty())
		return;
	setCurrentFile( fn );
	FuzzySaver fs(currentFile);
	fs.save(rugs);
}

void MainWindow::setCurrentFile(QString f)
{
	currentFile = f;
	setWindowTitle(QString("%1 - FuzzyRug").arg(currentFile));
}

void MainWindow::slotPrint()
{
	if(currentRug)
		RugPrinter::print(currentRug);
}


void MainWindow::slotUpdateSizeFromCanvas(QSizeF s)
{
	updateSpinBoxes();
}
