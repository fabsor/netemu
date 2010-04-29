#ifndef NETEMU_UI_H
#define NETEMU_UI_H

#include <QtGui/QWidget>
#include "ui_netemu_ui.h"

class netemu_ui : public QWidget
{
    Q_OBJECT

public:
    netemu_ui(QWidget *parent = 0);
    ~netemu_ui();

private:
    Ui::netemu_uiClass ui;
};

#endif // NETEMU_UI_H
