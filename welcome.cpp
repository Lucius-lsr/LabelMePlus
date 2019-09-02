#include "welcome.h"
#include "ui_welcome.h"

Welcome::Welcome(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Welcome)
{
    ui->setupUi(this);


    connect(ui->pushButton_quit,SIGNAL(clicked()),this,SLOT(close())); // 点击退出按钮则退出

    label2D = new Label2D;
    label2D->setWindowTitle("2D图片标注");
    connect(ui->pushButton_2D,SIGNAL(clicked()),label2D,SLOT(show())); // 点击打开2D标注界面
    connect(ui->pushButton_2D,SIGNAL(clicked()),this,SLOT(close())); // 同时关闭主界面

    label3D = new Label3D;
    label3D->setWindowTitle("3D图片标注");
    connect(ui->pushButton_3D,SIGNAL(clicked()),label3D,SLOT(show())); // 点击打开3D标注界面
    connect(ui->pushButton_3D,SIGNAL(clicked()),this,SLOT(close())); // 同时关闭主界面
}

Welcome::~Welcome()
{
    delete ui;
}
