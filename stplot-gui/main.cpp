#include "stplotwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    STPlotWindow w;
    w.show();
    return a.exec();
}
