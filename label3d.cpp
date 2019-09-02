#include "label3d.h"
#include "ui_label3d.h"

Label3D::Label3D(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Label3D)
{
    ui->setupUi(this);
}

Label3D::~Label3D()
{
    delete ui;
}
