#include "boring_game.h"

#include <QtGui>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    boring_game w;
    w.show();
    return a.exec();
}
