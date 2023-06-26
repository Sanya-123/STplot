#include "stplotwindow.h"

#include <QApplication>


extern "C" {
#include "stlink.h"
#include "usb.h"
// #include <chipid.h>
// #include <helper.h>
// #include "logging,h"
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    STPlotWindow w;
    w.show();

    stlink_t* sl = NULL;
    enum connect_type connect = CONNECT_HOT_PLUG;
    int32_t freq = 100000;
    sl = stlink_open_usb(UDEBUG, connect, NULL, freq);
    if (sl == NULL){ 
        return(-1); 
    }

    return a.exec();
}
