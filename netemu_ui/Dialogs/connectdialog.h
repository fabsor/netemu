#ifndef WORKINGDIALOG_H
#define WORKINGDIALOG_H

#include <QtGui/QDialog>
#include "ui_connectdialog.h"
#include "netemu_kaillera.h"

enum HostType {
	KailleraServer,
	NetEmuCloud
};

class ConnectDialog : public QDialog
{
    Q_OBJECT

public:
    ConnectDialog(QWidget *parent = 0, QString serverName = QString::null, QString address = QString::null, HostType type = KailleraServer, QString userName = QString::null);
    ~ConnectDialog();
    bool canceled();
    netemu_info *connectionInfo;

public slots:
	int exec();
private slots:
	void onCancelClicked();

private:
    Ui::WorkingDialogClass ui;
    QString serverName;
    QString address;
    HostType type;
    QString userName;
    bool isCanceled;
    void createActions();
    bool Connect();
    void ConnectSuccess(int status, struct netemu_info* server_connection);
};

#endif // WORKINGDIALOG_H
