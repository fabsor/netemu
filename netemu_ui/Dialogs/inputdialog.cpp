#include "inputdialog.h"

InputDialog::InputDialog(QWidget *parent, QString title, QString message)
    : QDialog(parent)
{
	ui.setupUi(this);
	this->setWindowTitle(title);
	ui.labelInfo->setText(message);
}

InputDialog::~InputDialog()
{

}

QString InputDialog::GetText()
{
	return ui.textAddress->text();
}
