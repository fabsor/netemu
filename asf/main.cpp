#include "netemu_ui.h"

#include <QtGui>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    netemu_ui w;
    w.show();
    return a.exec();
}
