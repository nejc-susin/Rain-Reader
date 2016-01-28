/*
* MyGraphicsView.cpp
*
*	Created in 2015
*		By: Nejc Susin
*/

#include <MyGraphicsView.h>

MyGraphicsView::MyGraphicsView(QWidget *parent) :
QGraphicsView(parent)
{
}

void MyGraphicsView::mousePressEvent(QMouseEvent * e)
{
	if (dragMode() == NoDrag)
	{
		if (placing)
		{
			placeFinished(lineX);
			scene()->removeItem(previousLine);
			placing = false;
			setMouseTracking(false);
		}
	}
	QGraphicsView::mousePressEvent(e);	
}
void MyGraphicsView::mouseMoveEvent(QMouseEvent * e)
{
	if (dragMode() == NoDrag)
	{
		QPointF coord = mapToScene(e->pos());
		QPaintEngine *pe = paintEngine();
		if (drawing)
		{
			path.push_back(e->pos());

			QRect r = QRect(QPoint(coord.x() - 2, coord.y() - 2), QPoint(coord.x() + 2, coord.y() + 2));
			trace.push_back(scene()->addEllipse(r, QPen(colorGreen), QBrush(colorGreen, Qt::SolidPattern)));
		}
		else if (placing)
		{
			lineX = coord.x();
			QRect r = QRect(coord.x() - 2, 0, 4, scene()->height() - 10);
			scene()->removeItem(previousLine);
			previousLine = scene()->addRect(r, QPen(colorBlue), QBrush(colorBlue));
		}
	}
	QGraphicsView::mouseMoveEvent(e);
}
void MyGraphicsView::mouseReleaseEvent(QMouseEvent * e)
{
	if (dragMode() == NoDrag)
	{
		if (drawing)
		{
			drawFinished(path);
			drawing = false;
			path.clear();
			while (trace.size() > 0)
			{
				scene()->removeItem(trace.back());
				trace.pop_back();
			}
		}
	}
	QGraphicsView::mouseReleaseEvent(e);
}

void MyGraphicsView::enableDrawing(bool enable)
{
	drawing = enable;
}
void MyGraphicsView::enablePlacing(bool enable)
{
	placing = enable;
}
void MyGraphicsView::enableDragging(bool enable)
{
	dragging = enable;
}