/*
* raingui.h
*
*	Created in 2015
*		By: Nejc Susin
*/

#ifndef RAINGUI_H
#define RAINGUI_H

#include <QtWidgets>
#include <QtWidgets/QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QDockWidget>
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QRect>
#include <QString>
#include <QDate>
#include <QFileDialog>
#include <QFileInfo>
#include <QMouseEvent>
#include <QRubberBand>
#include <QTabWidget>
#include <QListWidget>
#include <QTextStream>
#include <QMessageBox>
#include <QProgressBar>
#include <QApplication>

#include <MyGraphicsView.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include "opencv2/imgproc/imgproc.hpp"

#include "Processing.h"

namespace Ui {
	class rainGui;
}

class rainGui : public QMainWindow
{
	Q_OBJECT

public:
	explicit rainGui(QWidget *parent = 0);
	~rainGui();

public slots:
	void keyPressEvent(QKeyEvent *e);
	void resizeEvent(QResizeEvent *e);

private slots:
	void on_actionOpen_triggered();
	void on_actionZoomIn_triggered(); 
	void on_actionZoomOut_triggered();
	void on_actionFit_triggered();
	void on_actionSave_triggered();
	void exportButton_clicked();
	void changeStep(int step);
	void treshButtonA_clicked();
	void treshButtonB_clicked();
	void treshButtonAB_clicked();
	void treshChckView_changed(int state);
	void treshButtonApply_clicked();
	void treshSpinA1_valueChanged(int a1);
	void treshSpinA2_valueChanged(int a2);
	void treshSpinB1_valueChanged(int b1);
	void treshSpinB2_valueChanged(int b2);
	void aoiSpinTop_valueChanged(int pos);
	void aoiSpinRight_valueChanged(int pos);
	void aoiSpinBottom_valueChanged(int pos);
	void aoiTimespan_valueChanged(int hours);
	void aoiTime_timeChanged(QTime);
	void aoiDate_dateChanged(QDate);
	void aoiSpinLeft_valueChanged(int pos);
	void aoiCheckGrid_changed(int);
	void deleteHandler();
	void editOriginalCheck_changed(int state);
	void editExcludeButton_clicked();
	void editIncludeButton_clicked();
	void editInversionButton_clicked();
	void editResetButton_clicked();
	void editExclRemoveButton_clicked();
	void editInclRemoveButton_clicked();
	void editInvsRemoveButton_clicked();
	void editExclListWidget_selectionChanged(QListWidgetItem * a);
	void editInclListWidget_selectionChanged(QListWidgetItem * a);
	void editInvsListWidget_selectionChanged(QListWidgetItem * a);
	
	void exclusionAdded(QRect rect, QPointF fp, QPointF tp);
	void inclusionAdded(std::vector<QPoint> path);
	void inversionAdded(int x);
	void resultExportCombo_valueChanged(int idx);
	void massExport();

private:
	Ui::rainGui *ui;
	QGraphicsScene * scene;
	QGraphicsPixmapItem *currentImage;
	QWidget *progressWidget;

	Processing proc;

	int imgWidth, imgHeight;
	int treshA1, treshA2, treshB1, treshB2;
	cv::Mat input;
	cv::Rect aoi;
	int displayMode;	// which image to display - original, individual channels, tresholding... 
	string result;
	QString datetimeFormatPrint;
	QString datetimeFormatInternal;
	QString inputPath;
	QString outputPath;
	QString snapPath;

	QDateTime startDateTime;
	QPoint origin; 
	QRect qtRect;

	bool hasChanged;
	int confirmDialog(QString title, QString message);
	void scaleImage(double factor);
	void updateView();
	void extractWorker();
	string resultsWorker(int resolution, int timespan);
};

#endif // RAINGUI_H
