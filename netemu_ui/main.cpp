#include "mainwindow.h"

#include <QtGui>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("NetEmu");
    QCoreApplication::setOrganizationDomain("netemu.org");
    QCoreApplication::setApplicationName("NetEmu");
    MainWindow w;
    w.show();
    return a.exec();
}
