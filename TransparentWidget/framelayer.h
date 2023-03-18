#pragma once

#include <QWidget>
#include "system.h"

class FrameLayer : public QWidget
{
	Q_OBJECT

public:
	explicit FrameLayer(QWidget* parent = nullptr);
	virtual ~FrameLayer();

	void setPenColor(const QColor color) { mPenColor = color; }
	void setPenWidth(const qreal width) { mPenWidth = width; }
	void setDefaultSizeMargin(const int margin) { mDefaultSizeMargin = margin; }
	void setSystemWidgetsInfo(QList<QWidget*> systemWidgetsList);

	QColor getPenColor() { return mPenColor; }
	qreal getPenWidth() { return mPenWidth; }

signals: 
	void makeResize();
signals:
	void beginResizing(MoveType action);

protected:
	void paintEvent(QPaintEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;

private:
	void leftResizing(QMouseEvent* event, const QPoint delta);
	void rightResizing(QMouseEvent* event, const QPoint delta);
	void topResizing(QMouseEvent* event, const QPoint delta);
	void bottomResizing(QMouseEvent* event, const QPoint delta);
	void moving(QMouseEvent* event, const QPoint delta);

private:
	QWidget* mParent{ nullptr };
	int mDefaultSizeMargin{ 10 };
	int mMovingPanelWidth{ 0 };
	int mMovingPanelHeight{ 0 };
	MoveType mAction{ MoveType::NO_ACTION };
	const QPoint mInvalidPoint{ QPoint(-1, -1) };
	QPoint mGlobalPos{ QPoint(-1, -1) };
	QPoint mLocalPos{ QPoint(0, 0) };
	bool mMouseLeftButtonPressed{ false };
	bool mThisResizing{ false };
	qreal mPenWidth{ 1 };
	QColor mPenColor{ QColor(255,255,255,255) };
};
