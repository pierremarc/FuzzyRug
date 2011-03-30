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


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QGraphicsScene>

class RugItem;

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

	QGraphicsScene * rugScene;
	QMap<QString, RugItem*> rugs;
	RugItem * currentRug;

	QString currentFile;

	static MainWindow * MWInstance;

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();
	static MainWindow * MW();

private:
	Ui::MainWindow *ui;

	void blockSpinBoxesSignal(bool b);
	void updateSpinBoxes();
	void setCurrentFile(QString f);

public slots:
	void addPattern(QString path);
	void addRug(RugItem * ri);

private slots:
	void slotAddRug();
	void slotAddPattern();
	void slotImportPattern();

	void slotChangeCurrentRug();
	void slotChangeCurrentRug(const QString& rn);
	void slotSelectPattern();

	void slotChangeRugWidth(double v);
	void slotChangeRugHeight(double v);
	void slotChangeThreadHeight(double v);
	void slotChangePatternSize(double v);

	void slotZoom(int s);

	void slotOpen();
	void slotNew();
	void slotSave();
	void slotSaveAs();
	void slotPrint();

	void slotUpdateSizeFromCanvas(QSizeF s);
};

#endif // MAINWINDOW_H
