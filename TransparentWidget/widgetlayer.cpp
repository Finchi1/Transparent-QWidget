#include "widgetlayer.h"
#include <QResizeEvent>
#include <QLayout>
#include <QPainter>
#include <QList>
#include <QTime>

WidgetLayer::WidgetLayer(QWidget* parent)
	: QWidget(parent)
{
	this->setWindowFlags(Qt::FramelessWindowHint);
	this->setAttribute(Qt::WA_TranslucentBackground);
	this->setMinimumSize(200, 150);
	mResizingFrame = new FrameLayer(this);
	mResizingFrame->show();
	mBtnMinimize = new QPushButton("🗕", this);
	mBtnMaximize = new QPushButton("🗗", this);
	mBtnClose = new QPushButton("✕", this);
	mBtnClose->setCursor(Qt::CursorShape::ArrowCursor);
	mBtnMaximize->setCursor(Qt::CursorShape::ArrowCursor);
	mBtnMinimize->setCursor(Qt::CursorShape::ArrowCursor);
	mBtnClose->setFixedSize(mSystemButtonsSize);
	mBtnMinimize->setFixedSize(mSystemButtonsSize);
	mBtnMaximize->setFixedSize(mSystemButtonsSize);
	mBtnClose->setStyleSheet(QString{
		R"(
			QPushButton:!hover
			{
				background-color: rgba(%1, %2, %3, 1);
				color: white;
				border-width: 1px;
				border-color: rgba(%1, %2, %3, 1);
				font: bold;
				font-family: Times New Roman;
				font-size: 14;
				padding: 5px;
			}
			QPushButton:hover
			{
				background-color: red;
				color: white;
				border-width: 1px;
				border-color: rgba(%1, %2, %3, 1);
				font: bold;
				font-family: Times New Roman;
				font-size: 14;
				padding: 5px;
			}
		)" }.arg(mSystemPanelColor.red()).arg(mSystemPanelColor.green()).arg(mSystemPanelColor.blue()));

	mBtnMaximize->setStyleSheet(QString{
		R"(
			QPushButton:!hover
			{
				background-color: rgba(%1, %2, %3, 1);
				color: white;
				border-width: 1px;
				border-color: rgba(%1, %2, %3, 1);
				font: bold;
				font-family: Times New Roman;
				font-size: 14;
				padding: 5px;
			}
			QPushButton:hover
			{
				background-color: rgba(60, 60, 60, 1);
				color: white;
				border-width: 1px;
				border-color: rgba(%1, %2, %3, 1);
				font: bold;
				font-family: Times New Roman;
				font-size: 14;
				padding: 5px;
			}
		)" }.arg(mSystemPanelColor.red()).arg(mSystemPanelColor.green()).arg(mSystemPanelColor.blue()));
	mBtnMinimize->setStyleSheet(mBtnMaximize->styleSheet());
	auto widgetMaximize = [=]()
	{
		if (this->isMaximized())
		{
			this->showNormal();
			mResizingFrame->showNormal();
		}
		else
		{
			this->showMaximized();
			mResizingFrame->showMaximized();
		}
	};
	connect(mBtnMinimize, &QPushButton::clicked, this, &QWidget::showMinimized);
	connect(mBtnMaximize, &QPushButton::clicked, this, widgetMaximize);
	connect(mBtnClose, &QPushButton::clicked, qApp, &QApplication::quit);

	QList<QWidget*> systemWidgetsList{ mBtnMinimize, mBtnMaximize, mBtnClose };
	mResizingFrame->setSystemWidgetsInfo(systemWidgetsList);

	this->resize(640, 480);

	auto makeResize = [=]()
	{
		this->move(mResizingFrame->pos());
		this->resize(mResizingFrame->size());
		mIsFrameResizing = false;
		mAction = MoveType::NO_ACTION;
		this->repaint();
	};
	connect(mResizingFrame, &FrameLayer::makeResize, this, makeResize);

	auto frameInResizing = [=](MoveType action)
	{
		mIsFrameResizing = true;
		mAction = action;
		this->repaint();
	};
	connect(mResizingFrame, &FrameLayer::beginResizing, this, frameInResizing);
}

WidgetLayer::~WidgetLayer()
{}

void WidgetLayer::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	painter.fillRect(this->rect(), mMainRectColor);

	painter.setBrush(mSystemPanelColor);
	painter.setPen(Qt::NoPen);
	QRect systemPanel(0, 0, this->width(), mBtnClose->height() + 1);
	painter.drawRect(systemPanel);
	painter.setBrush(Qt::NoBrush);

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

	// Widget frame lines
	QLine leftLine(leftTopCorner, leftBottomCorner);
	QLine rightLine(rightTopCorner, rightBottomCorner);
	QLine topLine(leftTopCorner, rightTopCorner);
	QLine bottomLine(leftBottomCorner, rightBottomCorner);

	painter.setPen(QPen(mResizingFrame->getPenColor(), mResizingFrame->getPenWidth()));
	if (mIsFrameResizing)
		switch (mAction)
		{
		case MoveType::LEFT_RESIZING:
			painter.drawLine(rightLine);
			painter.drawLine(QLine(topRightOnMinimal, rightTopCorner));
			painter.drawLine(QLine(bottomRightOnMinimal, rightBottomCorner));
			break;
		case MoveType::RIGHT_RESIZING:
			painter.drawLine(leftLine);
			painter.drawLine(QLine(topLeftOnMinimal, leftTopCorner));
			painter.drawLine(QLine(bottomLeftOnMinimal, leftBottomCorner));
			break;
		case MoveType::TOP_RESIZING:
			painter.drawLine(bottomLine);
			painter.drawLine(QLine(leftBottomOnMinimal, leftBottomCorner));
			painter.drawLine(QLine(rightBottomOnMinimal, rightBottomCorner));
			break;
		case MoveType::BOTTOM_RESIZING:
			painter.drawLine(topLine);
			painter.drawLine(QLine(leftTopOnMinimal, leftTopCorner));
			painter.drawLine(QLine(rightTopOnMinimal, rightTopCorner));
			break;
		case MoveType::LEFT_TOP_RESIZING:
			painter.drawLine(rightBottomCorner, rightBottomCorner - QPoint(0, 40));
			painter.drawLine(rightBottomCorner, rightBottomCorner - QPoint(40, 0));
			break;
		case MoveType::LEFT_BOTTOM_RESIZING:
			painter.drawLine(rightTopCorner, rightTopCorner + QPoint(0, 40));
			painter.drawLine(rightTopCorner, rightTopCorner - QPoint(40, 0));
			break;
		case MoveType::RIGHT_TOP_RESIZING:
			painter.drawLine(leftBottomCorner, leftBottomCorner - QPoint(0, 40));
			painter.drawLine(leftBottomCorner, leftBottomCorner + QPoint(40, 0));
			break;
		case MoveType::RIGHT_BOTTOM_RESIZING:
			painter.drawLine(leftTopCorner, leftTopCorner + QPoint(0, 40));
			painter.drawLine(leftTopCorner, leftTopCorner + QPoint(40, 0));
			break;
		default:
			painter.drawLine(leftLine);
			painter.drawLine(topLine);
			painter.drawLine(rightLine);
			painter.drawLine(bottomLine);
			break;
		}
	painter.end();
}

void WidgetLayer::resizeEvent(QResizeEvent* event)
{
	mBtnClose->setGeometry(this->width() - mBtnClose->width() - 1, 1, 0, 0);
	mBtnMaximize->setGeometry(this->width() - mBtnClose->width() - mBtnMaximize->width() - 1, 1, 0, 0);
	mBtnMinimize->setGeometry(this->width() - mBtnClose->width() - mBtnMaximize->width() - mBtnMinimize->width() - 1, 1, 0, 0);
	QList<QWidget*> systemWidgetsList{ mBtnMinimize, mBtnMaximize, mBtnClose };
	mResizingFrame->setSystemWidgetsInfo(systemWidgetsList);
}
