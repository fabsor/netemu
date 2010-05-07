#ifndef INPUTDIALOG_H
#define INPUTDIALOG_H

#include <QtGui/QDialog>
#include "ui_inputdialog.h"

class InputDialog : public QDialog
{
    Q_OBJECT

public:
    InputDialog(QWidget *parent = 0, QString title = "Input", QString message = "Input");
    ~InputDialog();
    QString GetText();

private:
    Ui::InputDialogClass ui;
};

#endif // INPUTDIALOG_H
