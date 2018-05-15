#include "wateringgui.h"
#include <QApplication>

#define VER "1.0.1"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    WateringGUI w(VER);
    w.setWindowTitle(QString("CAGA - ver:%1").arg(VER));
    w.show();

    return a.exec();
}
