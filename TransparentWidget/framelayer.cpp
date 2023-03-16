#include "framelayer.h"
#include <QStackedLayout>
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QMouseEvent>
#include <QPainterPath>
#include <QTime>
#include "widgetlayer.h"

FrameLayer::FrameLayer(QWidget* parent)
	: QWidget(parent)
{
	mParent = parent;
	this->setWindowFlags(Qt::SplashScreen | Qt::FramelessWindowHint);
	this->setAttribute(Qt::WA_TranslucentBackground);
	this->setAttribute(Qt::WA_Hover);
	this->setMouseTracking(true);
	this->resize(mParent->size());
}

FrameLayer::~FrameLayer()
{}

void FrameLayer::setSystemWidgetsInfo(QList<QWidget*> systemWidgetsList)
{
	int minX{ 65535 }, maxHeight{ 0 };
	for (auto widget : systemWidgetsList)
	{
		if (widget->x() < minX)
			minX = widget->x();
		if (widget->height() > maxHeight)
			maxHeight = widget->height();
	}
	mMovingPanelWidth = minX;
	mMovingPanelHeight = maxHeight;
}

void FrameLayer::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);

	QPen pen(QPen(mPenColor, mPenWidth));
	painter.setPen(pen);

	// Widget corners points
	QPoint leftTopCorner(QPoint(0, 0));
	QPoint leftBottomCorner(QPoint(0, this->height() - painter.pen().width()));
	QPoint rightTopCorner(QPoint(this->width() - painter.pen().width(), 0));
	QPoint rightBottomCorner(QPoint(this->width() - painter.pen().width(), this->height() - painter.pen().width()));

	// Widget minimal size points
	QPoint leftTopOnMinimal(QPoint(0, this->minimumHeight()));
	QPoint leftBottomOnMinimal(QPoint(0, this->height() - this->minimumHeight()));
	QPoint rightTopOnMinimal(QPoint(this->width(), this->minimumHeight()));
	QPoint rightBottomOnMinimal(QPoint(this->width(), this->height() - this->minimumHeight()));
	QPoint topLeftOnMinimal(QPoint(this->minimumWidth(), 0));
	QPoint topRightOnMinimal(QPoint(this->width() - this->minimumWidth(), 0));
	QPoint bottomLeftOnMinimal(QPoint(this->minimumWidth(), this->height() - painter.pen().width()));
	QPoint bottomRightOnMinimal(QPoint(this->width() - this->minimumWidth(), this->height() - painter.pen().width()));

	// Widget middle-line points
	QPoint leftMiddle((leftTopCorner + leftBottomCorner) / 2);
	QPoint rightMiddle((rightTopCorner + rightBottomCorner) / 2);
	QPoint topMiddle((leftTopCorner + rightTopCorner) / 2);
	QPoint bottomMiddle((leftBottomCorner + rightBottomCorner) / 2);
	// Widget center point
	QPoint center((leftTopCorner + rightBottomCorner) / 2);

	// Widget frame lines
	QLine leftLine(leftTopCorner, leftBottomCorner);
	QLine rightLine(rightTopCorner, rightBottomCorner);
	QLine topLine(leftTopCorner, rightTopCorner);
	QLine bottomLine(leftBottomCorner, rightBottomCorner);

	// Widget corner-middle lines
	QLine leftTopLine(leftTopCorner, leftMiddle);
	QLine leftBottomLine(leftMiddle, leftBottomCorner);
	QLine rightTopLine(rightTopCorner, rightMiddle);
	QLine rightBottomLine(rightMiddle, rightBottomCorner);
	QLine topLeftLine(leftTopCorner, topMiddle);
	QLine topRightLine(topMiddle, rightTopCorner);
	QLine bottomLeftLine(leftBottomCorner, bottomMiddle);
	QLine bottomRightLine(bottomMiddle, rightBottomCorner);

	// Widget center lines
	QLine verticalCenterLine(topMiddle, bottomMiddle);
	QLine horizontalCenterLine(leftMiddle, rightMiddle);
	QLine verticalTopCenterLine(topMiddle, center);
	QLine verticalBottomCenterLine(center, bottomMiddle);
	QLine horizontalLeftCenterLine(leftMiddle, center);
	QLine horizontalRightCenterLine(center, rightMiddle);

	// Frame path
	QPainterPath framePath;
	framePath.addRect(QRect(leftTopCorner, leftBottomCorner + QPoint(mDefaultSizeMargin, 0)));
	framePath.addRect(QRect(rightTopCorner, rightBottomCorner - QPoint(mDefaultSizeMargin, 0)));
	framePath.addRect(QRect(leftTopCorner + QPoint(mDefaultSizeMargin + 1, 0), rightTopCorner + QPoint(-mDefaultSizeMargin, mDefaultSizeMargin)));
	framePath.addRect(QRect(leftBottomCorner + QPoint(mDefaultSizeMargin + 1, 0), rightBottomCorner - QPoint(mDefaultSizeMargin, mDefaultSizeMargin)));
	painter.fillPath(framePath, QColor(0, 0, 0, 1));

	// Moving panel rect
	QRect movingPanel(QPoint(0, 0), QPoint(mMovingPanelWidth, mMovingPanelHeight));
	painter.fillRect(movingPanel, QColor(0, 0, 0, 1));

	if (!mThisResizing || mAction == MoveType::NO_ACTION)
	{
		painter.drawLine(leftLine);
		painter.drawLine(rightLine);
		painter.drawLine(topLine);
		painter.drawLine(bottomLine);
	}
	else if (mThisResizing && mAction == MoveType::LEFT_TOP_RESIZING)
	{
		painter.drawLine(leftLine);
		painter.drawLine(topLine);
		pen.setStyle(Qt::DashLine);
		painter.setPen(pen);
		painter.drawLine(verticalCenterLine);
		painter.drawLine(horizontalCenterLine);
	}
	else if (mThisResizing && mAction == MoveType::RIGHT_TOP_RESIZING)
	{
		painter.drawLine(rightLine);
		painter.drawLine(topLine);
		pen.setStyle(Qt::DashLine);
		painter.setPen(pen);
		painter.drawLine(verticalCenterLine);
		painter.drawLine(horizontalCenterLine);
	}
	else if (mThisResizing && mAction == MoveType::LEFT_BOTTOM_RESIZING)
	{
		painter.drawLine(leftLine);
		painter.drawLine(bottomLine);
		pen.setStyle(Qt::DashLine);
		painter.setPen(pen);
		painter.drawLine(verticalCenterLine);
		painter.drawLine(horizontalCenterLine);
	}
	else if (mThisResizing && mAction == MoveType::RIGHT_BOTTOM_RESIZING)
	{
		painter.drawLine(rightLine);
		painter.drawLine(bottomLine);
		pen.setStyle(Qt::DashLine);
		painter.setPen(pen);
		painter.drawLine(verticalCenterLine);
		painter.drawLine(horizontalCenterLine);
	}
	else if (mThisResizing && mAction == MoveType::LEFT_RESIZING)
	{
		painter.drawLine(leftLine);
		painter.drawLine(topLeftLine);
		painter.drawLine(bottomLeftLine);
		painter.drawLine(topRightLine);
		painter.drawLine(bottomRightLine);
		pen.setStyle(Qt::DashLine);
		painter.setPen(pen);
		painter.drawLine(verticalCenterLine);
		painter.drawLine(horizontalCenterLine);
	}
	else if (mThisResizing && mAction == MoveType::RIGHT_RESIZING)
	{
		painter.drawLine(rightLine);
		painter.drawLine(topRightLine);
		painter.drawLine(bottomRightLine);
		painter.drawLine(topLeftLine);
		painter.drawLine(bottomLeftLine);
		pen.setStyle(Qt::DashLine);
		painter.setPen(pen);
		painter.drawLine(verticalCenterLine);
		painter.drawLine(horizontalCenterLine);
	}
	else if (mThisResizing && mAction == MoveType::TOP_RESIZING)
	{
		painter.drawLine(topLine);
		painter.drawLine(leftTopLine);
		painter.drawLine(rightTopLine);
		painter.drawLine(leftBottomLine);
		painter.drawLine(rightBottomLine);
		pen.setStyle(Qt::DashLine);
		painter.setPen(pen);
		painter.drawLine(verticalCenterLine);
		painter.drawLine(horizontalCenterLine);
	}
	else if (mThisResizing && mAction == MoveType::BOTTOM_RESIZING)
	{
		painter.drawLine(bottomLine);
		painter.drawLine(leftBottomLine);
		painter.drawLine(rightBottomLine);
		painter.drawLine(leftTopLine);
		painter.drawLine(rightTopLine);
		pen.setStyle(Qt::DashLine);
		painter.setPen(pen);
		painter.drawLine(verticalCenterLine);
		painter.drawLine(horizontalCenterLine);
	}
	painter.end();
}

void FrameLayer::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		mGlobalPos = event->globalPos();
		mMouseLeftButtonPressed = true;
		if (mAction != MoveType::NO_ACTION && mAction!=MoveType::MOVING)
			emit beginResizing(mAction);
		if (mAction == MoveType::MOVING)
			this->setCursor(Qt::ClosedHandCursor);
	}
	QWidget::mousePressEvent(event);
}

void FrameLayer::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		mGlobalPos = mInvalidPoint;
		mMouseLeftButtonPressed = false;
		emit makeResize();
		if (mAction == MoveType::MOVING)
			this->setCursor(Qt::OpenHandCursor);
	}
	mThisResizing = false;
	this->repaint();
	QWidget::mouseReleaseEvent(event);
}

void FrameLayer::mouseMoveEvent(QMouseEvent* event)
{
	if (!mMouseLeftButtonPressed)
	{
		if (event->pos().x() <= 0 + mDefaultSizeMargin && event->pos().y() <= 0 + mDefaultSizeMargin)
		{
			mAction = MoveType::LEFT_TOP_RESIZING;
			if (this->cursor() != Qt::SizeFDiagCursor)
				this->setCursor(Qt::SizeFDiagCursor);
		}
		else if (event->pos().x() <= 0 + mDefaultSizeMargin && event->pos().y() >= this->height() - mDefaultSizeMargin)
		{
			mAction = MoveType::LEFT_BOTTOM_RESIZING;
			if (this->cursor() != Qt::SizeBDiagCursor)
				this->setCursor(Qt::SizeBDiagCursor);
		}
		else if (event->pos().x() >= this->width() - mDefaultSizeMargin && event->pos().y() <= 0 + mDefaultSizeMargin)
		{
			mAction = MoveType::RIGHT_TOP_RESIZING;
			if (this->cursor() != Qt::SizeBDiagCursor)
				this->setCursor(Qt::SizeBDiagCursor);
		}
		else if (event->pos().x() >= this->width() - mDefaultSizeMargin && event->pos().y() >= this->height() - mDefaultSizeMargin)
		{
			mAction = MoveType::RIGHT_BOTTOM_RESIZING;
			if (this->cursor() != Qt::SizeFDiagCursor)
				this->setCursor(Qt::SizeFDiagCursor);
		}
		else if (event->pos().x() <= 0 + mDefaultSizeMargin)
		{
			mAction = MoveType::LEFT_RESIZING;
			if (this->cursor() != Qt::SizeHorCursor)
				this->setCursor(Qt::SizeHorCursor);
		}
		else if (event->pos().x() >= this->width() - mDefaultSizeMargin)
		{
			mAction = MoveType::RIGHT_RESIZING;
			if (this->cursor() != Qt::SizeHorCursor)
				this->setCursor(Qt::SizeHorCursor);
		}
		else if (event->pos().y() <= 0 + mDefaultSizeMargin)
		{
			mAction = MoveType::TOP_RESIZING;
			if (this->cursor() != Qt::SizeVerCursor)
				this->setCursor(Qt::SizeVerCursor);
		}
		else if (event->pos().y() >= this->height() - mDefaultSizeMargin)
		{
			mAction = MoveType::BOTTOM_RESIZING;
			if (this->cursor() != Qt::SizeVerCursor)
				this->setCursor(Qt::SizeVerCursor);
		}
		else if (event->pos().x() > 0 && event->pos().x() < mMovingPanelWidth && event->pos().y() > 0 && event->pos().y() < mMovingPanelHeight)
		{
			mAction = MoveType::MOVING;
			if (this->cursor() != Qt::OpenHandCursor)
				this->setCursor(Qt::OpenHandCursor);
		}
		else
		{
			mAction = MoveType::NO_ACTION;
			if (this->cursor() != Qt::ArrowCursor)
				this->setCursor(Qt::ArrowCursor);
		}
	}
	else
	{
		if (mGlobalPos == mInvalidPoint)
			return QWidget::mouseMoveEvent(event);
		const QPoint delta = event->globalPos() - mGlobalPos;
		switch (mAction)
		{
		case MoveType::LEFT_RESIZING:
			leftResizing(event, delta);
			break;
		case MoveType::RIGHT_RESIZING:
			rightResizing(event, delta);
			break;
		case MoveType::TOP_RESIZING:
			topResizing(event, delta);
			break;
		case MoveType::BOTTOM_RESIZING:
			bottomResizing(event, delta);
			break;
		case MoveType::LEFT_TOP_RESIZING:
			leftResizing(event, delta);
			topResizing(event, delta);
			break;
		case MoveType::LEFT_BOTTOM_RESIZING:
			leftResizing(event, delta);
			bottomResizing(event, delta);
			break;
		case MoveType::RIGHT_TOP_RESIZING:
			rightResizing(event, delta);
			topResizing(event, delta);
			break;
		case MoveType::RIGHT_BOTTOM_RESIZING:
			rightResizing(event, delta);
			bottomResizing(event, delta);
			break;
		case MoveType::MOVING:
			moving(event, delta);
			break;
		default:
			break;
		}
	}
}

void FrameLayer::resizeEvent(QResizeEvent* event)
{
	this->setMinimumSize(mParent->minimumSize());
}

void FrameLayer::leftResizing(QMouseEvent* event, const QPoint delta)
{
	if (width() - delta.x() > this->minimumSize().width())
	{
		resize(width() - delta.x(), height());
		move(pos() + QPoint(delta.x(), 0));
		mGlobalPos = event->globalPos();
		mThisResizing = true;
	}
	else
		return;
}

void FrameLayer::rightResizing(QMouseEvent* event, const QPoint delta)
{
	if (width() + delta.x() > this->minimumSize().width())
	{
		resize(width() + delta.x(), height());
		mGlobalPos = event->globalPos();
		mThisResizing = true;
	}
	else
		return;
}

void FrameLayer::topResizing(QMouseEvent* event, const QPoint delta)
{
	if (height() - delta.y() > this->minimumSize().height())
	{
		resize(width(), height() - delta.y());
		move(pos() + QPoint(0, delta.y()));
		mGlobalPos = event->globalPos();
		mThisResizing = true;
	}
	else
		return;
}

void FrameLayer::bottomResizing(QMouseEvent* event, const QPoint delta)
{
	if (height() + delta.y() > this->minimumSize().height())
	{
		resize(width(), height() + delta.y());
		mGlobalPos = event->globalPos();
		mThisResizing = true;
	}
	else
		return;
}

void FrameLayer::moving(QMouseEvent* event, const QPoint delta)
{
	this->move(pos() + QPoint(delta.x(), delta.y()));
	mParent->move(this->pos());
	mGlobalPos = event->globalPos();
}
