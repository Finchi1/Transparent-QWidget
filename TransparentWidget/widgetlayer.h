#pragma once

#include <QWidget>
#include "framelayer.h"
#include "system.h"
#include <QPushButton>
#include <QApplication>

class WidgetLayer : public QWidget
{
	Q_OBJECT

public:
	explicit WidgetLayer(QWidget* parent = nullptr);
	virtual ~WidgetLayer();

	void setDefaultSizeMargin(const int margin) { mResizingFrame->setDefaultSizeMargin(margin); }
	void setFrameColor(const QColor color) { mResizingFrame->setPenColor(color); }
	void setFrameWidth(const qreal width) { mResizingFrame->setPenWidth(width); }
	void setMainRectColor(const QColor color) {}
	void setBlurBehind(bool blured) { mBlurBehind = blured; }

protected:
	void paintEvent(QPaintEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;
	bool nativeEvent(const QByteArray& eventType, void* message, qintptr* result) override;

private:
	void setWindowBlur(HWND hWnd);

private:
	FrameLayer* mResizingFrame{ nullptr };
	QPushButton* mBtnClose{ nullptr };
	QPushButton* mBtnMaximize{ nullptr };
	QPushButton* mBtnMinimize{ nullptr };
	bool mIsFrameResizing{ false };
	MoveType mAction{ MoveType::NO_ACTION };
	QColor mMainRectColor{ QColor(45,45,45,200) };
	QColor mSystemPanelColor{ QColor(25, 25, 25, 255) };
	QSize mSystemButtonsSize{ QSize(45, 40) };
	bool mFixedSize{ false };
	bool mBlurBehind{ false };
};
