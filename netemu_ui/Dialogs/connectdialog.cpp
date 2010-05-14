#include "connectdialog.h"

#define DEFAULT_PORT	27888
//#include "netemu_p2p.h"

ConnectDialog::ConnectDialog(QWidget *parent, QString serverName, QString address, HostType type, QString userName)
    : QDialog(parent)
{
	QString connectingString;
	ui.setupUi(this);
	this->createActions();
	this->serverName = serverName;
	this->address = address;
	this->type = type;
	this->userName = userName;
	this->setWindowTitle("NetEmu - Connecting to " + serverName);

	connectingString = "Connecting to ";// (type == KailleraServer ? "server '" : "cloud '") + name + "'";
	connectingString += (this->type == KailleraServer ? "server " : "cloud ");
	connectingString += "'" + this->serverName + "'";
	ui.labelConnecting->setText(connectingString);
	//ui.labelConnecting->setText("Connecting to " + (type == KailleraServer ? "server " : "cloud ") + name);
}

void ConnectResponse(int status, struct netemu_kaillera* server_connection, void *arg) {
	ConnectDialog* dialog = (ConnectDialog*)arg;
	if(dialog != NULL && !dialog->canceled()) {
		dialog->accept();
	}
	else {
		dialog->reject();
	}
}

bool ConnectDialog::canceled()
{
	return this->isCanceled;
}

void ConnectDialog::onCancelClicked()
{
	this->isCanceled = true;
	this->reject();
}

void ConnectDialog::createActions()
{
	connect(ui.buttonCancel, SIGNAL(clicked()), this, SLOT(onCancelClicked()));
}

int ConnectDialog::exec()
{
	this->show();
	if(this->Connect())
		return QDialog::Accepted;
	else
		return QDialog::Rejected;
	return 0;
}

bool ConnectDialog::Connect()
{
	QByteArray addressBytes, usernameBytes;
	char* address;
	short port;
	bool portConversionSuccess;
	netemu_sockaddr_in addr;

	QStringList stringList = this->address.split(':', QString::SkipEmptyParts);
	if(stringList.length() > 2)
	{
		qDebug("Error in address format. Address contained more than 1 colon.");
		qDebug(this->address.toLatin1().data());
		return false;
	}
	else if(stringList.length() == 1)
	{
		/* No port supplied, use default */
		port = DEFAULT_PORT;
	}
	else
	{
		port = (short)stringList.value(1).toInt(&portConversionSuccess, 10);
	}

	addressBytes = stringList.value(0).toLatin1();
	usernameBytes = this->userName.toLatin1();
	address = addressBytes.data();

	if(this->type == KailleraServer)
	{
		/* TODO: Figure out why connect_async wont work. netemu_util_prepare_receiver errors on the first malloc for some reason */
		/*kaillera_communication_connect_async(&addr, sizeof(addr), "W00t", this->userName.toLatin1().data(), ConnectResponse, this);*/
		this->connectionInfo = netemu_kaillera_create(usernameBytes.data(),"W00t", 1);
		this->connectionInfo = netemu_kaillera_connect(this->connectionInfo, netemu_inet_addr("127.0.0.1"),0,netemu_inet_addr(address), netemu_htons(port));

		if(this->connectionInfo == NULL) {
			qDebug("Connection error");
			return false;
		}
		return true;
	}
	return false;
}

void ConnectDialog::ConnectSuccess(int status, struct netemu_kaillera* server_connection)
{

}

ConnectDialog::~ConnectDialog()
{

}
