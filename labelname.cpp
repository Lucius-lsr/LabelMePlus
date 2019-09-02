#include "labelname.h"
#include "ui_labelname.h"
#include <QMessageBox>

Labelname::Labelname(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Labelname)
{
    ui->setupUi(this);


    connect(ui->buttonBox,SIGNAL(accepted()),this,SLOT(submit()));
}

Labelname::~Labelname()
{
    delete ui;
}
void Labelname::submit()
{
    {
        QMessageBox::warning(this,"","标签名不能为空");
    }
    else
    {
        name = ui->labelName->text();
        this->close();
    }
}

