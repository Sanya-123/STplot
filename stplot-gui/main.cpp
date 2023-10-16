#include "stplotwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qRegisterMetaType< QVector<uint8_t> >( "QVector<uint8_t>" );
    qRegisterMetaType< uint32_t >( "uint32_t" );
    STPlotWindow w;
    w.show();
    return a.exec();
}
