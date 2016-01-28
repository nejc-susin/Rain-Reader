/*
* MyGraphicsView.h
*
*	Created in 2015
*		By: Nejc Susin
*/

#ifndef MYGRAPHICSVIEW_H
#define MYGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QMouseEvent>
#include <QPaintEngine>
#include <QPainter>
#include <QPainterPath>
#include <QGraphicsPathItem>

class MyGraphicsView : public QGraphicsView
{
	Q_OBJECT

public:
	MyGraphicsView(QWidget *parent = 0);

	void enableDrawing(bool enable);
	void enablePlacing(bool enable);
	void enableDragging(bool enable);

public slots:
	void mousePressEvent(QMouseEvent * e);
	void mouseMoveEvent(QMouseEvent * e);
	void mouseReleaseEvent(QMouseEvent * e);

private:
	std::vector<QPoint> path;
	std::vector<QGraphicsEllipseItem*> trace;
	QGraphicsRectItem * previousLine;
	int lineX;

	QColor colorGreen = QColor(0, 255, 0, 200);
	QColor colorBlue = QColor(0, 0, 255, 200);

	bool drawing;
	bool placing;
	bool dragging;

signals:
	void drawFinished(std::vector<QPoint>);
	void placeFinished(int);
	void deletePressed();
};

#endif // MYGRAPHICSVIEW_H
