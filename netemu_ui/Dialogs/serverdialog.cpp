#include "serverdialog.h"

ServerDialog::ServerDialog(QWidget *parent, netemu_info *serverInfo)
    : QMainWindow(parent)
{
	ui.setupUi(this);
}

ServerDialog::~ServerDialog()
{

}
