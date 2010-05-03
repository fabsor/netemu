#ifndef WORKINGDIALOG_H
#define WORKINGDIALOG_H

#include <QtGui/QDialog>
#include "ui_connectdialog.h"

enum HostType {
	KailleraServer,
	NetEmuCloud
};

class ConnectDialog : public QDialog
{
    Q_OBJECT

public:
    ConnectDialog(QWidget *parent = 0, QString name = QString::null, QString address = QString::null, HostType type = KailleraServer);
    ~ConnectDialog();

private:
    Ui::WorkingDialogClass ui;
    QString name;
    QString address;
    HostType type;
    void Connect();
    void ConnectSuccess(int status, struct netemu_info* server_connection);
};

#endif // WORKINGDIALOG_H
