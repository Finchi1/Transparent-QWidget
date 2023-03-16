#include "widgetlayer.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    WidgetLayer w;
    w.show();
    return a.exec();
}