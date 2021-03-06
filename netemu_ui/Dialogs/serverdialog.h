#ifndef SERVERDIALOG_H
#define SERVERDIALOG_H

#include <QtGui/QMainWindow>
#include "ui_serverdialog.h"
#include "netemu_kaillera.h"

class ServerDialog : public QMainWindow
{
    Q_OBJECT

public:
    ServerDialog(QWidget *parent = 0, netemu_kaillera *serverInfo = NULL);
    ~ServerDialog();
    void ChatCallBack(char *user, char *message);
    void OnUserListChanged();
    void OnGameListChanged();

private slots:
	void OnButtonSendChatClick();

private:
    Ui::ServerDialogClass ui;
    netemu_kaillera *serverInfo;
    void InitializeUserList();
    void CreateActions();
};

#endif // SERVERDIALOG_H
