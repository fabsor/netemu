#include "connectdialog.h"
#include "netemu_kaillera.h"
//#include "netemu_p2p.h"

ConnectDialog::ConnectDialog(QWidget *parent, QString name, QString address, HostType type)
    : QDialog(parent)
{
	QString connectingString;
	ui.setupUi(this);
	this->name = name;
	this->address = address;
	this->type = type;
	this->setWindowTitle("NetEmu - Connecting to " + name);

	connectingString = "Connecting to ";// (type == KailleraServer ? "server '" : "cloud '") + name + "'";
	connectingString += (this->type == KailleraServer ? "server " : "cloud ");
	connectingString += "'" + this->name + "'";
	ui.labelConnecting->setText(connectingString);
	//ui.labelConnecting->setText("Connecting to " + (type == KailleraServer ? "server " : "cloud ") + name);
	this->Connect();
}

void foo(int status, struct netemu_info* server_connection) {
	int i;
	i = 5;
	i++;
}

void ConnectDialog::Connect()
{
	QByteArray addressBytes;
	short port;
	bool portConversionSuccess;
	struct netemu_sockaddr_in addr;
	QStringList stringList = this->address.split(':', QString::SkipEmptyParts);
	if(stringList.length() != 2) {
		qDebug("Error in address format.");
		qDebug(this->address.toLatin1().data());
		return;
	}

	addressBytes = this->address.toLatin1();
	addr.addr = netemu_inet_addr(addressBytes.data());
	addr.port = netemu_htons((short)stringList.value(1).toInt(&portConversionSuccess, 10));
	addr.family = NETEMU_AF_INET;
	if(this->type == KailleraServer)
	{
		kaillera_communication_connect_async(&addr, sizeof(addr), "W00t", "Emil", this->ConnectSuccess);
	}
	//kaillera_communication_connect_async(&addr,sizeof(addr),EMUNAME,PLAYERNAME,login_success);
}

void ConnectDialog::ConnectSuccess(int status, struct netemu_info* server_connection)
{

}

ConnectDialog::~ConnectDialog()
{

}
