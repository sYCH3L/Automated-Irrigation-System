#include "wateringgui.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    WateringGUI w;
    w.show();

    return a.exec();
}
