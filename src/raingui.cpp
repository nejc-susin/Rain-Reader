/*
* raingui.cpp
*
*	Created in 2015
*		By: Nejc Susin
*/

#include "raingui.h"
#include "ui_raingui.h"

int resolution = 60;
int timespan = 24;

rainGui::rainGui(QWidget *parent) :
QMainWindow(parent),
ui(new Ui::rainGui)
{
	ui->setupUi(this);
	ui->resultExportCombo->addItem("1 day", 1440);
	ui->resultExportCombo->addItem("12 hour", 720);
	ui->resultExportCombo->addItem("2 hours", 120);
	ui->resultExportCombo->addItem("1 hour", 60);
	ui->resultExportCombo->addItem("30 minutes", 30);
	ui->resultExportCombo->addItem("10 minutes", 10);
	ui->resultExportCombo->addItem("5 minutes", 5);
	ui->resultExportCombo->setCurrentIndex(3);

	ui->tabWidget->setTabEnabled(1, false);
	ui->tabWidget->setTabEnabled(2, false);
	ui->tabWidget->setTabEnabled(3, false);
	ui->tabWidget->setTabEnabled(4, false);

	progressWidget = new QWidget(ui->centralWidget);
	progressWidget->setGeometry(width()-590, 10, 200, 100);
	progressWidget->setWindowFlags(Qt::X11BypassWindowManagerHint | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
	progressWidget->setAutoFillBackground(true);

	QLabel *pleaseWait = new QLabel(progressWidget);
	pleaseWait->setText("Processing...");
	pleaseWait->setFont(QFont("sans", 15));
	pleaseWait->setGeometry(0, 0, progressWidget->width(), progressWidget->height());
	pleaseWait->setAlignment(Qt::AlignCenter);
	
	progressWidget->hide();

	// default thresholding values
	treshA1 = 160;
	treshA2 = 150;
	treshB1 = 110;
	treshB2 = 126;
	ui->treshSpinA1->setValue(treshA1);
	ui->treshSpinA2->setValue(treshA2);
	ui->treshSpinB1->setValue(treshB1);
	ui->treshSpinB2->setValue(treshB2);

	datetimeFormatPrint = "yyyy-MM-dd";
	datetimeFormatInternal = "yyyy-MM-dd HH:mm:ss.zzz";

	connect(ui->openButton, SIGNAL(clicked()), this, SLOT(on_actionOpen_triggered()));
	connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(changeStep(int)));

	connect(ui->aoiSpinTop, SIGNAL(valueChanged(int)), this, SLOT(aoiSpinTop_valueChanged(int)));
	connect(ui->aoiSpinRight, SIGNAL(valueChanged(int)), this, SLOT(aoiSpinRight_valueChanged(int)));
	connect(ui->aoiSpinBottom, SIGNAL(valueChanged(int)), this, SLOT(aoiSpinBottom_valueChanged(int)));
	connect(ui->aoiSpinLeft, SIGNAL(valueChanged(int)), this, SLOT(aoiSpinLeft_valueChanged(int)));
	connect(ui->aoiTimespan, SIGNAL(valueChanged(int)), this, SLOT(aoiTimespan_valueChanged(int)));
	connect(ui->aoiDate, SIGNAL(dateChanged(QDate)), this, SLOT(aoiDate_dateChanged(QDate)));
	connect(ui->aoiTime, SIGNAL(timeChanged(QTime)), this, SLOT(aoiTime_timeChanged(QTime)));
	connect(ui->aoiCheckGrid, SIGNAL(stateChanged(int)), this, SLOT(aoiCheckGrid_changed(int)));

	connect(ui->treshButtonA, SIGNAL(clicked()), this, SLOT(treshButtonA_clicked()));
	connect(ui->treshButtonB, SIGNAL(clicked()), this, SLOT(treshButtonB_clicked()));
	connect(ui->treshButtonAB, SIGNAL(clicked()), this, SLOT(treshButtonAB_clicked()));
	connect(ui->treshButtonApply, SIGNAL(clicked()), this, SLOT(treshButtonApply_clicked()));
	connect(ui->treshChckView, SIGNAL(stateChanged(int)), this, SLOT(treshChckView_changed(int)));

	connect(ui->treshSpinA1, SIGNAL(valueChanged(int)), this, SLOT(treshSpinA1_valueChanged(int)));
	connect(ui->treshSpinA2, SIGNAL(valueChanged(int)), this, SLOT(treshSpinA2_valueChanged(int)));
	connect(ui->treshSpinB1, SIGNAL(valueChanged(int)), this, SLOT(treshSpinB1_valueChanged(int)));
	connect(ui->treshSpinB2, SIGNAL(valueChanged(int)), this, SLOT(treshSpinB2_valueChanged(int)));

	connect(ui->editOriginalCheck, SIGNAL(stateChanged(int)), this, SLOT(editOriginalCheck_changed(int)));
	connect(ui->editExcludeButton, SIGNAL(clicked()), this, SLOT(editExcludeButton_clicked()));
	connect(ui->editIncludeButton, SIGNAL(clicked()), this, SLOT(editIncludeButton_clicked()));
	connect(ui->editInversionButton, SIGNAL(clicked()), this, SLOT(editInversionButton_clicked()));
	connect(ui->editResetButton, SIGNAL(clicked()), this, SLOT(editResetButton_clicked()));
	connect(ui->editExclRemoveButton, SIGNAL(clicked()), this, SLOT(editExclRemoveButton_clicked()));
	connect(ui->editInclRemoveButton, SIGNAL(clicked()), this, SLOT(editInclRemoveButton_clicked()));
	connect(ui->editInvsRemoveButton, SIGNAL(clicked()), this, SLOT(editInvsRemoveButton_clicked()));

	connect(ui->editExclListWidget, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(editExclListWidget_selectionChanged(QListWidgetItem *)));
	connect(ui->editInclListWidget, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(editInclListWidget_selectionChanged(QListWidgetItem *)));
	connect(ui->editInvsListWidget, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(editInvsListWidget_selectionChanged(QListWidgetItem *)));


	connect(ui->resultExportCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(resultExportCombo_valueChanged(int)));
	connect(ui->resultExportButton, SIGNAL(clicked()), this, SLOT(exportButton_clicked()));
	//connect(ui->resultExportButton, SIGNAL(clicked()), this, SLOT(massExport()));	// special mass export option

	connect(ui->graphicsView, SIGNAL(drawFinished(std::vector<QPoint>)), this, SLOT(inclusionAdded(std::vector<QPoint>)));
	connect(ui->graphicsView, SIGNAL(placeFinished(int)), this, SLOT(inversionAdded(int)));
	connect(ui->graphicsView, SIGNAL(rubberBandChanged(QRect, QPointF, QPointF)), this, SLOT(exclusionAdded(QRect, QPointF, QPointF)));
	
	ui->aoiDate->setDate(QDate::currentDate());
	ui->aoiTime->setTime(QTime(7,0,0,0));
}

rainGui::~rainGui()
{
	delete ui;

}
void rainGui::keyPressEvent(QKeyEvent * e)
{
	if (e->key() == Qt::Key_Delete)
	{
		deleteHandler();
	}
	if (e->key() == Qt::Key_Plus)
	{
		on_actionZoomIn_triggered();
	}
	if (e->key() == Qt::Key_Minus)
	{
		on_actionZoomOut_triggered();
	}
}
void rainGui::resizeEvent(QResizeEvent *e)
{
	progressWidget->setGeometry(width() - 590, 10, 200, 100);
	
}
void rainGui::on_actionOpen_triggered()
{
	QString format = "jpg";
	QString filename = QFileDialog::getOpenFileName(this, tr("Open"), inputPath, tr("All Files (*)").arg(format.toUpper()).arg(format));
	if (!filename.isEmpty())
	{	
		editResetButton_clicked();
		progressWidget->show();
		QApplication::setOverrideCursor(Qt::WaitCursor);
		qApp->processEvents();
		QFileInfo(filename).fileName();
		input = cv::imread(filename.toStdString());
		displayMode = 0;
		hasChanged = true;
		scene = new QGraphicsScene();

		cv::Mat img = input.clone();
		imgWidth = img.cols;
		imgHeight = img.rows;
		proc.setImage(img);
		cv::cvtColor(input, img, CV_BGR2RGB);
		currentImage = scene->addPixmap(QPixmap::fromImage(QImage((uchar*)img.data, img.cols, img.rows, img.step, QImage::Format_RGB888)));

		ui->graphicsView->setScene(scene);
		ui->graphicsView->show();

		aoi = proc.getAoiRect();

		ui->aoiSpinTop->setMaximum(imgHeight);
		ui->aoiSpinRight->setMaximum(imgWidth);
		ui->aoiSpinBottom->setMaximum(imgHeight);
		ui->aoiSpinLeft->setMaximum(imgWidth);
		ui->aoiSpinTop->setValue(aoi.y);
		ui->aoiSpinRight->setValue(aoi.x + aoi.width);
		ui->aoiSpinBottom->setValue(aoi.y + aoi.height);
		ui->aoiSpinLeft->setValue(aoi.x);

		inputPath = QFileInfo(filename).absolutePath();
		ui->resultLocation->setText(QFileInfo(filename).baseName());
		setWindowTitle("Rain Reader: " + QFileInfo(filename).fileName());

		ui->tabWidget->setTabEnabled(1, true);
		ui->tabWidget->setTabEnabled(2, true);
		ui->tabWidget->setTabEnabled(3, true);
		ui->tabWidget->setTabEnabled(4, true);
		ui->actionSave->setEnabled(true);
		ui->actionZoomIn->setEnabled(true);
		ui->actionZoomOut->setEnabled(true);
		ui->actionFit->setEnabled(true);

		rainGui::on_actionFit_triggered();

		QApplication::restoreOverrideCursor();
		ui->tabWidget->setCurrentIndex(0);
		ui->tabWidget->setCurrentIndex(4);
	}
}

void rainGui::on_actionZoomIn_triggered()
{
	ui->graphicsView->scale(1.25, 1.25);
}

void rainGui::on_actionZoomOut_triggered()
{
	ui->graphicsView->scale(0.75, 0.75);
}

void rainGui::on_actionFit_triggered()
{
	ui->graphicsView->fitInView(ui->graphicsView->sceneRect(), Qt::KeepAspectRatio);
}

void rainGui::on_actionSave_triggered()
{
	if (currentImage)
	{
		QString format = "png";
		QString fname = "/" +  ui->resultLocation->text();

		QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"), snapPath + fname, tr("%1 Files (*.%2);;All Files (*)").arg(format.toUpper()).arg(format));
		if (!fileName.isEmpty())
		{
			progressWidget->show();
			QApplication::setOverrideCursor(Qt::WaitCursor);
			qApp->processEvents();
			snapPath = QFileInfo(fileName).absolutePath();
			currentImage->pixmap().save(fileName, format.toLatin1().constData());
			progressWidget->hide();
			QApplication::restoreOverrideCursor();
		}
	}
}
void rainGui::changeStep(int step)
{
	switch (step)
	{
	case 1:
		displayMode = 4;
		rainGui::updateView();
		break;
	case 2:
		if (ui->aoiCheckGrid->isChecked())
		{
			displayMode = 11;
		}
		else
		{
			displayMode = 7;
		}
		rainGui::updateView();
		break;
	case 3:
		if (ui->editOriginalCheck->isChecked())
		{
			displayMode = 8;
		}
		else
		{
			displayMode = 9;
		}
		if (hasChanged)
		{
			extractWorker();
		}
		rainGui::updateView();
		break;
	case 4:
		progressWidget->show();
		QApplication::setOverrideCursor(Qt::WaitCursor);
		qApp->processEvents();
		displayMode = 10;
		//displayMode = 12; //DEBUG

		resolution = ui->resultExportCombo->currentData().toInt();
		string datestr = startDateTime.toString(datetimeFormatInternal).toStdString();
		if (hasChanged)
		{
			result = proc.entireProcess(resolution, datestr, timespan, new int[4] {treshA1, treshA2, treshB1, treshB2});
			ui->resultText->setText(QString::fromStdString(result));
			hasChanged = false;
		}
		updateView();
		progressWidget->hide();
		QApplication::restoreOverrideCursor();
		break;
	}
}
void rainGui::updateView()
{
	cv::Mat img;
	switch (displayMode)
	{
	case 0:		//original RGB
		img = proc.getOriginal();
		break;
	case 1:		//original channel L
		img = proc.getChannel(0);
		cv::cvtColor(img, img, CV_GRAY2RGB);
		break;
	case 2:		//original channel a
		img = proc.getChannel(1);
		cv::cvtColor(img, img, CV_GRAY2RGB);
		break;
	case 3:		//original channel b
		img = proc.getChannel(2);
		cv::cvtColor(img, img, CV_GRAY2RGB);
		break;
	case 4:		//after tresholding combined
		img = proc.getBinary(0);
		cv::cvtColor(img, img, CV_GRAY2RGB);
		break;
	case 5:		//after tresholding a
		img = proc.getBinary(1);
		cv::cvtColor(img, img, CV_GRAY2RGB);
		break;
	case 6:		//after tresholding b
		img = proc.getBinary(2);
		cv::cvtColor(img, img, CV_GRAY2RGB);
		break;
	case 7:		//with AOI
		img = proc.getAoi(1, 0, 0, 0);
		break;
	case 8:		//extracted curve on original 
		img = proc.getExtracted(0, ui->editExclListWidget->currentRow(), ui->editInclListWidget->currentRow(), ui->editInvsListWidget->currentRow());
		break;
	case 9:		//extracted curve on corrected binary 
		img = proc.getExtracted(1, ui->editExclListWidget->currentRow(), ui->editInclListWidget->currentRow(), ui->editInvsListWidget->currentRow());
		break;
	case 10:		//final curve
		img = proc.getFinal();
		break;
	case 11:		//with AOI and grid
		proc.makeGrid(timespan);
		img = proc.getAoi(1, 1, 0, 0);
		break;
	case 12:		//DEBUG 
		img = proc.getDebug();
		break;
	}
	QImage qim = QImage((uchar*)img.data, img.cols, img.rows, img.step, QImage::Format_RGB888);
	currentImage->setPixmap(QPixmap::fromImage(qim));
}
void rainGui::exportButton_clicked()
{
	QString format = "txt";
	QString name = ui->resultLocation->text();
	QString dt = startDateTime.toString(datetimeFormatPrint);

	if (name.isEmpty())
	{
		name = "rainread";
	}
	if (name != dt)
	{
		name.append("_").append(dt);
	}
	name.prepend("/");

	QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"), outputPath.append(name), tr("%1 Files (*.%2);;All Files (*)").arg(format.toUpper()).arg(format));
	if (!fileName.isEmpty())
	{
		outputPath = QFileInfo(fileName).absolutePath();;
		QFile file(fileName);
		if (!file.open(QIODevice::WriteOnly)) {
			std::cerr << "Cannot open file for writing: "
				<< qPrintable(file.errorString()) << std::endl;
			return;
		}
		QTextStream out(&file);
		out << QString::fromStdString(result);
	}
}
void rainGui::massExport()
{
	std::vector<string> outputNames = {"result-daily.txt", "result-hourly.txt", "result-halfhourly.txt", "result-5min.txt"};
	int outputResolutions[] = { 1440, 60, 30, 5 };
	if (outputPath.length() < 1)
	{
		outputPath = QFileDialog::getExistingDirectory(this, tr("Save directory"), 0);
	}
	for (int i = 0; i < outputNames.size(); i++)
	{
		result = proc.results(outputResolutions[i], startDateTime.toString(datetimeFormatInternal).toStdString(), timespan);
		QString tmp = outputPath + "/" + QString::fromStdString(outputNames[i]);
		QFile file(tmp);
		if (!file.open(QIODevice::Append)) {
			std::cerr << "Cannot open file for appending: "
				<< qPrintable(file.errorString()) << std::endl;
			return;
		}
		QTextStream out(&file);
		out << QString::fromStdString(result);
	}
}
void rainGui::treshButtonAB_clicked()
{
	ui->treshChckView->isChecked() ? displayMode = 0 : displayMode = 4;
	rainGui::updateView();
}
void rainGui::treshButtonA_clicked()
{
	ui->treshChckView->isChecked() ? displayMode = 2 : displayMode = 5;
	rainGui::updateView();
}
void rainGui::treshButtonB_clicked()
{
	ui->treshChckView->isChecked() ? displayMode = 3 : displayMode = 6;
	rainGui::updateView();
}
void rainGui::treshChckView_changed(int state)
{
	switch (displayMode)
	{
	case 0:
		displayMode = 4;
		break;
	case 2:
		displayMode = 5;
		break;
	case 3:
		displayMode = 6;
		break;
	case 4:
		displayMode = 0;
		break;
	case 5:
		displayMode = 2;
		break;
	case 6:
		displayMode = 3;
		break;
	}
	rainGui::updateView();
}
void rainGui::treshButtonApply_clicked()
{
	progressWidget->show();
	hasChanged = true;
	QApplication::setOverrideCursor(Qt::WaitCursor);
	qApp->processEvents();
	proc.threshold(treshA1, treshA2, treshB1, treshB2);
	rainGui::updateView();
	progressWidget->hide();
	QApplication::restoreOverrideCursor();
}
void rainGui::treshSpinA1_valueChanged(int a1)
{
	treshA1 = a1;
}
void rainGui::treshSpinA2_valueChanged(int a2)
{
	treshA2 = a2;
}
void rainGui::treshSpinB1_valueChanged(int b1)
{
	treshB1 = b1;
}
void rainGui::treshSpinB2_valueChanged(int b2)
{
	treshB2 = b2;
}
void rainGui::aoiSpinTop_valueChanged(int pos)
{
	hasChanged = true;
	aoi.height += (aoi.y - pos);
	aoi.y = pos;
	proc.setAoi(aoi);
	updateView();
}
void rainGui::aoiSpinRight_valueChanged(int pos)
{
	hasChanged = true;
	aoi.width = pos - aoi.x;
	proc.setAoi(aoi);
	updateView();
}
void rainGui::aoiSpinBottom_valueChanged(int pos)
{
	hasChanged = true;
	aoi.height = pos - aoi.y;
	proc.setAoi(aoi);
	updateView();
}
void rainGui::aoiSpinLeft_valueChanged(int pos)
{
	hasChanged = true;
	aoi.width += aoi.x - pos;
	aoi.x = pos;
	proc.setAoi(aoi);
	updateView();
}
void rainGui::aoiTimespan_valueChanged(int hours)
{
	hasChanged = true;
	timespan = hours;
	proc.makeGrid(timespan);
	updateView();
}
void rainGui::aoiCheckGrid_changed(int val)
{
	if (val != 0)
	{
		displayMode = 11;
	}
	else
	{
		displayMode = 7;
	}
	updateView();
}
void rainGui::deleteHandler()
{
	QString msg = "Are you sure you wish to remove ";
	QString title = "Confirm deletion";
	if (ui->editExclListWidget->currentRow() >= 0 && ui->editExclListWidget->hasFocus())
	{
		msg.append(ui->editExclListWidget->currentItem()->text() + "?");
		if (confirmDialog(title, msg) == QMessageBox::Yes)
		{
			editExclRemoveButton_clicked();
		}
	}
	else if (ui->editInvsListWidget->currentRow() >= 0 && ui->editInvsListWidget->hasFocus())
	{
		msg.append(ui->editInvsListWidget->currentItem()->text() + "?");
		if (confirmDialog(title, msg) == QMessageBox::Yes)
		{
			editInvsRemoveButton_clicked();
		}
	}
	else if (ui->editInclListWidget->currentRow() >= 0 && ui->editInclListWidget->hasFocus())
	{
		msg.append(ui->editInclListWidget->currentItem()->text() + "?");
		if (confirmDialog(title, msg) == QMessageBox::Yes)
		{
			editInclRemoveButton_clicked();
		}
	}
	
}
int rainGui::confirmDialog(QString title, QString message)
{
	return QMessageBox::information(this, title, message, QMessageBox::No | QMessageBox::Yes);
}
void rainGui::editOriginalCheck_changed(int state)
{
	if (state)
	{
		displayMode = 8;
	}
	else
	{
		displayMode = 9;
	}
	updateView();
}
void rainGui::editExcludeButton_clicked()
{
	ui->graphicsView->setDragMode(QGraphicsView::RubberBandDrag);
}
void rainGui::editIncludeButton_clicked()
{
	ui->graphicsView->setDragMode(QGraphicsView::NoDrag);
	ui->graphicsView->enableDrawing(true);
}
void rainGui::editInversionButton_clicked()
{
	ui->graphicsView->setDragMode(QGraphicsView::NoDrag);
	ui->graphicsView->enablePlacing(true);
	ui->graphicsView->setMouseTracking(true);
}
void rainGui::editResetButton_clicked()
{
	if (ui->editExclListWidget->count() > 0 || ui->editInclListWidget->count() > 0 || ui->editInvsListWidget->count() > 0)
	{
		ui->editExclListWidget->clear();
		ui->editInclListWidget->clear();
		ui->editInvsListWidget->clear();
		proc.resetCorrections();
		hasChanged = true;
		extractWorker();
	}
}
void rainGui::editExclRemoveButton_clicked()
{
	int row = ui->editExclListWidget->currentRow();
	if (row >= 0)
	{
		ui->editExclListWidget->takeItem(row);
		proc.removeExclusion(row);
		hasChanged = true;
		extractWorker();
	}
}
void rainGui::editInclRemoveButton_clicked()
{
	int row = ui->editInclListWidget->currentRow();
	if (row >= 0)
	{
		ui->editInclListWidget->takeItem(row);
		proc.removeInclusion(row);
		hasChanged = true;
		extractWorker();
	}
}
void rainGui::editInvsRemoveButton_clicked()
{
	int row = ui->editInvsListWidget->currentRow();
	if (row >= 0)
	{
		ui->editInvsListWidget->takeItem(row);
		proc.removeInversion(row);
		hasChanged = true;
		extractWorker();
	}
}
void rainGui::editExclListWidget_selectionChanged(QListWidgetItem * a)
{
	ui->editInclListWidget->setCurrentRow(-1);
	ui->editInvsListWidget->setCurrentRow(-1);
	updateView();
}
void rainGui::editInclListWidget_selectionChanged(QListWidgetItem * a)
{
	ui->editExclListWidget->setCurrentRow(-1);
	ui->editInvsListWidget->setCurrentRow(-1);
	updateView();
}
void rainGui::editInvsListWidget_selectionChanged(QListWidgetItem * a)
{
	ui->editExclListWidget->setCurrentRow(-1);
	ui->editInclListWidget->setCurrentRow(-1);
	updateView();
}
void rainGui::exclusionAdded(QRect rect, QPointF fp, QPointF tp)
{
	if (!rect.x())
	{
		QPointF topLeft = ui->graphicsView->mapToScene(qtRect.topLeft());
		QPointF bottomRight = ui->graphicsView->mapToScene(qtRect.bottomRight());
		cv::Rect cvRect = cv::Rect(topLeft.x(), topLeft.y(), bottomRight.x() - topLeft.x(), bottomRight.y() - topLeft.y());

		ui->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);

		if (proc.addExclusion(cvRect))
		{
			new QListWidgetItem(tr(("Exclusion " + to_string(ui->editExclListWidget->count() + 1)).c_str()), ui->editExclListWidget);
			hasChanged = true;
			extractWorker();
		}
	}
	else
	{
		qtRect = rect;
	}
}

void rainGui::inclusionAdded(std::vector<QPoint> path)
{
	std::vector<cv::Point> points;
	for (unsigned i = 0; i < path.size(); ++i)
	{
		QPointF p = ui->graphicsView->mapToScene(path[i]);
		points.push_back(cv::Point(p.x(), p.y()));
	}
	proc.addInclusion(points);

	ui->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
	new QListWidgetItem(tr(("Curve " + to_string(ui->editInclListWidget->count() + 1)).c_str()), ui->editInclListWidget);

	hasChanged = true;
	extractWorker();
}
void rainGui::inversionAdded(int x)
{
	proc.addInversion(x);
	ui->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
	new QListWidgetItem(tr(("Inversion " + to_string(ui->editInvsListWidget->count() + 1)).c_str()), ui->editInvsListWidget);
	hasChanged = true;
	extractWorker();
}
void rainGui::aoiDate_dateChanged(QDate date)
{
	startDateTime.setDate(date);
	hasChanged = true;
}
void rainGui::aoiTime_timeChanged(QTime time)
{
	startDateTime.setTime(time);
	hasChanged = true;
}
void rainGui::resultExportCombo_valueChanged(int idx)
{
	resolution = ui->resultExportCombo->currentData().toInt();
	result = proc.results(resolution, startDateTime.toString(datetimeFormatInternal).toStdString(), timespan);
	ui->resultText->setText(QString::fromStdString(result));
}
void rainGui::extractWorker()
{
	progressWidget->show();
	QApplication::setOverrideCursor(Qt::WaitCursor);
	qApp->processEvents();
	proc.extract();
	updateView();
	progressWidget->hide();
	QApplication::restoreOverrideCursor();
}
