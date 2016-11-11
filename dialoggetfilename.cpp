#include "dialoggetfilename.h"
#include "ui_dialoggetfilename.h"

DialogGetFileName::DialogGetFileName(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogGetFileName)
{
    ui->setupUi(this);
}

DialogGetFileName::~DialogGetFileName()
{
    delete ui;
}

void DialogGetFileName::on_buttonBox_accepted()
{
    fileName = ui->lineEdit->text();
}
