#include "dialoglog.h"
#include "ui_dialoglog.h"

DialogLog::DialogLog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogLog)
{
    ui->setupUi(this);
}

DialogLog::~DialogLog()
{
    delete ui;
}
