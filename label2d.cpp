#include "label2d.h"
#include "ui_label2d.h"
#include "labelname.h"
#include "labels.h"
#include "redocommand.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QImageReader>
#include <QDir>
#include <QDebug>
#include <QVector>
#include <QStyle>
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QSignalMapper>
#include <QBuffer>
#include <QUndoCommand>

/*-------该函数用来检测两点是否足够接近---------*/
bool isNear(QPoint p1, QPoint p2)
{
    if((p1.x()-p2.x())>5)
        return false;
    if(p2.x()-p1.x()>5)
        return false;
    if(p1.y()-p2.y()>5)
        return false;
    if(p2.y()-p1.y()>5)
        return false;
    return true;
}


Label2D::Label2D(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Label2D)
{
    ui->setupUi(this);
    /*-----------一些必须的变量初始化------------------*/
    color = Qt::red;
    showed = new QImage;
    state=0;
    hasSaved=true;
    penSize =1;
    penShape=0;
    segState=0;
    undoStack = new QUndoStack;
    mag=false;
    /*------------初始化绘制状态----------------*/
    rect=false;
    rectPainting=false;
    poly=false;
    polyPainting=false;
    polyfinish=false;
    /*---------------状态栏------------------*/
    ui->currentPicture->setReadOnly(true);
    ui->currentDir->setReadOnly(true);
    ui->coordinateX->setText("---");
    ui->coordinateY->setText("---");
    /*---------------颜色选项------------------*/
    QRadioButton** colorButtons = new QRadioButton*[6];
    colorButtons[0]=ui->radioButton_red;
    colorButtons[1]=ui->radioButton_ora;
    colorButtons[2]=ui->radioButton_yel;
    colorButtons[3]=ui->radioButton_gre;
    colorButtons[4]=ui->radioButton_pur;
    colorButtons[5]=ui->radioButton_blu;
    QSignalMapper* m = new QSignalMapper(this); // 创建信号映射器
    for(int i=0;i<6;i++)
    {
        connect(colorButtons[i],SIGNAL(pressed()),m,SLOT(map())); // 点击打开
        m->setMapping(colorButtons[i], i);
    }
    connect(m, SIGNAL(mapped(int)), this, SLOT(colorchange(int)));

    /*----------------------菜单栏和工具栏-------------------------*/
    connect(ui->actionOpen,SIGNAL(triggered()),this,SLOT(openSinglePicture())); // 点击打开
    connect(ui->toolButton_open,SIGNAL(clicked()),this,SLOT(openSinglePicture())); // 打开的工具栏按钮
    connect(ui->actionOpenFiles,SIGNAL(triggered()),this,SLOT(openPictureGroup())); // 点击打开文件夹
    connect(ui->toolButton_openFiles,SIGNAL(clicked()),this,SLOT(openPictureGroup())); // 打开文件夹的工具栏按钮
    connect(ui->actionNext,SIGNAL(triggered()),this,SLOT(gotoNext())); // 下一张的菜单按钮
    connect(ui->actionPrevious,SIGNAL(triggered()),this,SLOT(gotoPrevious())); // 上一张的菜单按钮
    connect(ui->toolButton_next,SIGNAL(clicked()),this,SLOT(gotoNext())); // 下一张的工具栏按钮
    connect(ui->toolButton_previous,SIGNAL(clicked()),this,SLOT(gotoPrevious())); // 上一张的工具栏按钮
    connect(ui->actionSave,SIGNAL(triggered()),this,SLOT(savelabeled())); // 保存的菜单按钮
    connect(ui->toolButton_save,SIGNAL(clicked()),this,SLOT(savelabeled())); // 保存的工具栏按钮
    connect(ui->actionClose,SIGNAL(triggered()),this,SLOT(closelabeled())); // 关闭的菜单按钮
    connect(ui->toolButton_close,SIGNAL(clicked()),this,SLOT(closelabeled())); // 关闭的工具栏按钮
    connect(ui->actionDeletelabel,SIGNAL(triggered()),this,SLOT(deletelabel())); // 删除标记的菜单按钮
    connect(ui->toolButton_closeLabel,SIGNAL(clicked()),this,SLOT(deletelabel())); // 删除标记的工具栏按钮
    connect(ui->buttonRect,SIGNAL(pressed()),this,SLOT(rectMode())); // 矩形绘图
    connect(ui->buttonPoly,SIGNAL(pressed()),this,SLOT(polyMode())); // 多边形绘图
    connect(ui->actionUndo,SIGNAL(triggered()),this,SLOT(unDo()));
    connect(ui->actionRedo,SIGNAL(triggered()),this,SLOT(reDo()));
    connect(ui->toolButton_undo,SIGNAL(clicked()),this,SLOT(unDo()));
    connect(ui->toolButton_redo,SIGNAL(clicked()),this,SLOT(reDo()));
    connect(ui->toolButton_magnify,SIGNAL(clicked()),this,SLOT(magnifying()));
    /*---------------工具栏图标设置------------------*/
    ui->toolButton_next->setArrowType(Qt::DownArrow);
    ui->toolButton_previous->setArrowType(Qt::UpArrow);
    ui->toolButton_next->setStyleSheet("QToolButton{ background: rgb(68, 69, 73); color: rgb(0, 160, 0);}");
    ui->toolButton_previous->setStyleSheet("QToolButton{ background: rgb(68, 69, 73); color: rgb(0, 160, 0);}");
    QIcon iconSave,iconOpenFile,iconOpenFiles,iconClose,iconRedo,iconUndo,iconCloseLabel;
    iconSave.addPixmap( style()->standardPixmap(QStyle::SP_DialogSaveButton ));
    iconOpenFile.addPixmap((style()->standardPixmap(QStyle::SP_DirOpenIcon)));
    iconOpenFiles.addPixmap(style()->standardPixmap(QStyle::SP_DirHomeIcon));
    iconClose.addPixmap(style()->standardPixmap(QStyle::SP_BrowserStop));
    iconRedo.addPixmap(style()->standardPixmap(QStyle::SP_ArrowRight));
    iconUndo.addPixmap(style()->standardPixmap(QStyle::SP_ArrowLeft));
    iconCloseLabel.addPixmap(style()->standardPixmap(QStyle::SP_DialogCancelButton));
    ui->toolButton_save->setIcon(iconSave);
    ui->toolButton_open->setIcon(iconOpenFile);
    ui->toolButton_openFiles->setIcon(iconOpenFiles);
    ui->toolButton_close->setIcon(iconClose);
    ui->toolButton_redo->setIcon(iconRedo);
    ui->toolButton_undo->setIcon(iconUndo);
    ui->toolButton_closeLabel->setIcon(iconCloseLabel);
    ui->toolButton_save->setIconSize(QSize(50,50));
    ui->toolButton_open->setIconSize(QSize(50,50));
    ui->toolButton_openFiles->setIconSize(QSize(50,50));
    ui->toolButton_close->setIconSize(QSize(50,50));
    ui->toolButton_redo->setIconSize(QSize(50,50));
    ui->toolButton_undo->setIconSize(QSize(50,50));
    ui->toolButton_magnify->setIconSize(QSize(50,50));

    /*----------------------状态栏选择图片和标签----------------------*/
    connect(ui->nameList,SIGNAL(currentRowChanged(int)),this,SLOT(itemChange(int))); // 改变当前的图片
    connect(ui->labelList,SIGNAL(currentRowChanged(int)),this,SLOT(labelChange(int))); // 改变选择的标签
    connect(ui->labelList,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(labelChange(QListWidgetItem*)));
    /*----------------------变更标注模式-------------------------*/
    connect(ui->tabWidget,SIGNAL(currentChanged(int)),this,SLOT(modeChanged(int)));
    connect(ui->penSize1,SIGNAL(pressed()),this,SLOT(setPenSize1()));
    connect(ui->penSize2,SIGNAL(pressed()),this,SLOT(setPenSize2()));
    connect(ui->penSize3,SIGNAL(pressed()),this,SLOT(setPenSize3()));
    connect(ui->penShapeRect,SIGNAL(pressed()),this,SLOT(setPenShapeRect()));
    connect(ui->penShapeCircle,SIGNAL(pressed()),this,SLOT(setPenShapeCircle()));
    connect(ui->pushButton_finish,SIGNAL(clicked()),this,SLOT(segFinish()));
    connect(ui->pushButton_cancel,SIGNAL(clicked()),this,SLOT(segCanceled()));
}

Label2D::~Label2D()
{
    delete showed;
    delete ui;
    delete undoStack;
    delete labeled;
}

void Label2D::openSinglePicture()
{
    /*------------------------打开单张图片--------------------------*/
    QString path = QFileDialog::getOpenFileName(this,"选择图片",".","Image Files(*.jpg *.png *.gif *.bmp *.jpeg)"); // 支持多种图片格式
    if(path=="") // 取消打开
        return;
    QImageReader reader(path);
    reader.setDecideFormatFromContent(true);
    QImage* img=new QImage;
    if(!reader.read(img)) // 加载图像失败
    {
        delete img;
        return;
    }

    QFileInfo fileinfo=QFileInfo(path); // 首先添加到图片列表中
    InfoList.append(fileinfo);
    LabelInfoList.append(emptyLabelInfos);
    labeled = img;
    showImage(labeled); // 展示图片
    setCurrentInfo(fileinfo); // 显示当前图片信息

    /*----------------------重置列表------------------------------*/
    ui->nameList->clear();
    QStringList pictureNames; // 创建向量保存每个图片的名称和完全的路径
    for(int i=0;i<InfoList.size();i++)
    {
        pictureNames.append(InfoList[i].fileName());
    }
    ui->nameList->addItems(pictureNames);
    ui->nameList->setCurrentRow(InfoList.size()-1); // 默认打开最后一张
    currentItem = InfoList.size()-1;

    /*--------------------自动加载标签-------------------*/
    QFile textfile(fileinfo.path()+"/"+fileinfo.baseName()+".txt");
    if(textfile.open(QIODevice::ReadOnly | QIODevice::Text)) // 如果有标签数据则加载标签数据
    {
        QTextStream in(&textfile);
        int labelNumber;
        in >> labelNumber;
        for(int i=0;i<labelNumber;i++)
        {
            int thisType;
            in>>thisType;
            if(thisType==0)
            {
                QString name;
                QString colorName;
                int top,left,buttom,right;
                in >> name >> colorName >> top >>left >>buttom >>right;
                QColor thisColor=stringToColor(colorName);
                QPoint p1(left,top),p2(right,buttom);
                QRect thisRect(p1,p2);
                LabelInfoList[currentItem].append(LabelInfo(0,emptyPoly,thisRect,thisColor,name));
            }
            else
            {
                QString name;
                QString colorName;
                in >> name >> colorName;
                QColor thisColor=stringToColor(colorName);
                int edges;
                in >> edges;
                QVector<QPoint> points(edges);
                for(int t=0;t<edges;t++)
                {
                    int x,y;
                    in>>x>>y;
                    points[t]=QPoint(x,y);
                }
                LabelInfoList[currentItem].append(LabelInfo(1,points,emptyRect,thisColor,name));
            }
        }
        updateLabelList(currentItem);
        updateLabeledPicture(currentItem);
    }
    delete img;
}

void Label2D::setCurrentInfo(QFileInfo fileInfo)
{
    QString file_name, file_path;
    file_name = fileInfo.fileName(); // 图片名
    file_path = fileInfo.absolutePath(); // 图片路径
    ui->currentPicture->setText(file_name);
    ui->currentDir->setText(file_path);

    //dirPath=file_path; // 将目录设置为当前图片所在的目录
}

void Label2D::showImage(QImage* img)
{
    /*----------------------设置图片大小和位置--------------------------*/
    QImage* scaledimg=new QImage; // 分别保存原图和缩放之后的图片
    int Owidth=img->width(),Oheight=img->height();
    int width,height;
    ui->label_picture->setGeometry(10,30,550,550);
    double ratio=Owidth/Oheight; // 长宽比
    if(ratio>=1)
    {
        *scaledimg=img->scaled(550,static_cast<int>(550/ratio),Qt::KeepAspectRatio);
    }
    else
    {
        *scaledimg=img->scaled(static_cast<int>(550*ratio),550,Qt::KeepAspectRatio);
    }
    /*----------------------处理偏差--------------------------*/
    width=scaledimg->width();
    height=scaledimg->height();
    if(width>=height)
    {
        bias=QPoint(0,(width-height)/2);
    }
    else
    {
        bias=QPoint((height-width)/2,0);
    }

    /*----------------------显示图像--------------------------*/
    beginLabel(scaledimg);
    ui->label_picture->setPixmap(QPixmap::fromImage(*scaledimg));

}

void Label2D::openPictureGroup()
{
    QString path = QFileDialog::getExistingDirectory(this,"选择文件夹",".");
    QDir dir(path);
    if(path=="") // 取消打开
        return;
    dir.setFilter(QDir::Files);
    QFileInfoList list = dir.entryInfoList();

    currentItem = InfoList.size()-1; // 先将条目移动到最后一个
    for (int i = 0; i < list.size(); i++)
    {
        QFileInfo thisInfo = list.at(i);
        if(thisInfo.suffix()=="jpg"||thisInfo.suffix()=="jpeg"||thisInfo.suffix()=="bmp" //保证只读入图片
                ||thisInfo.suffix()=="gif"||thisInfo.suffix()=="png")
        {
            InfoList.append(thisInfo);
            LabelInfoList.append(emptyLabelInfos);
            currentItem++;

            /*--------------------自动加载标签-------------------*/
            QFile textfile(thisInfo.path()+"/"+thisInfo.baseName()+".txt");
            if(textfile.open(QIODevice::ReadOnly | QIODevice::Text)) // 如果有标签数据则加载标签数据
            {
                QTextStream in(&textfile);
                int labelNumber;
                in >> labelNumber;
                for(int i=0;i<labelNumber;i++)
                {
                    int thisType;
                    in>>thisType;
                    if(thisType==0)
                    {
                        QString name;
                        QString colorName;
                        int top,left,buttom,right;
                        in >> name >> colorName >> top >>left >>buttom >>right;
                        QColor thisColor=stringToColor(colorName);
                        QPoint p1(left,top),p2(right,buttom);
                        QRect thisRect(p1,p2);
                        LabelInfoList[currentItem].append(LabelInfo(0,emptyPoly,thisRect,thisColor,name));
                    }
                    else
                    {
                        QString name;
                        QString colorName;
                        in >> name >> colorName;
                        QColor thisColor=stringToColor(colorName);
                        int edges;
                        in >> edges;
                        QVector<QPoint> points(edges);
                        for(int t=0;t<edges;t++)
                        {
                            int x,y;
                            in>>x>>y;
                            points[t]=QPoint(x,y);
                        }
                        LabelInfoList[currentItem].append(LabelInfo(1,points,emptyRect,thisColor,name));
                    }
                }
            }
        }
    }
    if(InfoList.size()==0)
        return;
    updateLabelList(currentItem);
    openPicture(InfoList[currentItem]);
    updateLabeledPicture(currentItem);


    /*----------------------重置列表------------------------------*/
    ui->nameList->clear();
    QStringList pictureNames; // 创建向量保存每个图片的名称和完全的路径
    for(int i=0;i<InfoList.size();i++)
    {
        pictureNames.append(InfoList[i].fileName());
    }
    ui->nameList->addItems(pictureNames);
    ui->nameList->setCurrentRow(currentItem);
}

void Label2D::openPicture(QFileInfo info)
{
    QString file_path=info.filePath();
    QImageReader reader(file_path);
    reader.setDecideFormatFromContent(true);
    QImage* img=new QImage;
    if(!reader.read(img)) // 加载图像失败
    {
        delete img;
    }
    else
    {
        showImage(img); // 展示图片                
        setCurrentInfo(info); // 显示当前图片信息
        updateLabelList(currentItem);
    }
}

void Label2D::itemChange(int item)
{
    if(item<0) // 避免自动设置为-1导致程序崩溃
        return;
    if(hasSaved==false) // 提示保存
    {
        if(QMessageBox::Yes==QMessageBox::question(this,"是否保存","是否保存修改？"))
        {
            savelabeled();
        }
        hasSaved=true;
    }
    currentItem = item;
    openPicture(InfoList[item]);
    updateLabeledPicture(currentItem);
    ui->nameList->setCurrentRow(currentItem);
    undoStack->clear(); // 清除撤销栈
}

void Label2D::beginLabel(QImage *img)
{
    labeled = img;
    QImage temp;
    temp = *labeled;
    *showed = temp;
    rect=true;
    poly=true;
}

void Label2D::mousePressEvent(QMouseEvent *e)
{
    /*----------------------画矩形---------------------*/
    if(state==0)
    {
        if(rect==true&&rectPainting==false) // 第一个点
        {
            previousPos = e->pos().operator-=(QPoint(90,30));
            rectPainting=true;
        }
        else if(rect==true&&rectPainting==true) // 画矩形完毕
        {
            rect=false;
        }
    }
    /*----------------------画多边形---------------------*/
    if(state==1)
    {
        if(poly==true&&polyPainting==false) // 第一个点
        {
            previousPos = e->pos().operator-=(QPoint(90,30));
            polyPainting=true;
            polyRecorder.append(previousPos);
        }
        else if(poly==true&&polyPainting==true&&polyfinish==false) // 画中间的点
        {
            previousPos = e->pos().operator-=(QPoint(90,30));
            polyRecorder.append(previousPos);
        }
        else if(poly==true&&polyPainting==true&&polyfinish==true) // 画回第一个点
        {
            poly=false;
        }
    }
    /*----------------------分割标注---------------------*/
    if(state==2)
    {
        oldImage = *showed;
        if(segState==0) // 还没开始画的状态
        {
            beginSegm(); // 再次确定已经创建文件夹并展示原图
            segState=1;
        }
        else if(segState==2) // 开始画之后继续画
        {
            segState=1;
        }
        update();
    }
}

void Label2D::mouseReleaseEvent(QMouseEvent *e)
{
    if(state==2)
    {
        if(segState==1) // 放下鼠标
        {
            segState=2;
            undoStack->push(new SegAddDeleteCommand(showed, oldImage, *showed));
        }

    }
}

void Label2D::mouseMoveEvent(QMouseEvent *e)
{
    /*----------------显示鼠标坐标-------------------*/
    QPoint location;
    location=e->pos().operator-=(QPoint(90,30));
    if(location.x()>=0&&location.x()<=550&&location.y()>=0&&location.y()<=550)
    {
        ui->coordinateX->setText(QString::number(location.x()));
        ui->coordinateY->setText(QString::number(location.y()));
    }
    /*----------------------画矩形---------------------*/
    if(state==0)
    {
        if(rect==true&&rectPainting==true)
        {
            currentPos=location;
        }
    }
    /*----------------------画多边形---------------------*/
    if(state==1)
    {
        polyfinish=false;
        if(poly==true&&polyPainting==true)
        {
            currentPos=location;
        }
        if(polyRecorder.size()>1)
        {
            if(isNear(location,polyRecorder[0])) // 形成闭合图形
            {
                polyfinish=true;
            }
        }
    }
    /*----------------------分割标注---------------------*/
    if(state==2)
    {
        currentPos=location;
    }

    if(mag&&InfoList.size()>0) // 放大镜模式
    {
        if(location.x()>=0&&location.x()<=550&&location.y()>=0&&location.y()<=550)
        {
            ui->magnifyingGlass->setGeometry(e->pos().x()-100,e->pos().y()-100,200,200);
            QImage magnified=(ui->label_picture->pixmap())->toImage().copy(location.x()-40-bias.x(),location.y()-40-bias.y(),80,80);
            magnified=magnified.scaled(200,200);
            ui->magnifyingGlass->setPixmap(QPixmap::fromImage(magnified));
        }
        else
        {
            ui->magnifyingGlass->setGeometry(10,140,16,20);
            ui->magnifyingGlass->clear();
        }
    }

}

void Label2D::paintEvent(QPaintEvent* e)//重写窗体重绘事件
{
    if(InfoList.size()==0) // 防止未加载时就绘图
        return;
    /*----------------------画矩形---------------------*/
    if(state==0)
    {
        if(rect==true&&rectPainting==true) // 正在画矩形
        {
            QImage temp1 = *showed; // 用临时变量保存图片以展示
            QPainter painter(&temp1);
            painter.setRenderHints(QPainter::Antialiasing);
            QPen pen(color, 3);
            painter.setPen(pen);
            QRect rectangle(previousPos-bias,currentPos-bias); // 消除偏差后的坐标
            painter.drawRect(rectangle);
            ui->label_picture->setPixmap(QPixmap::fromImage(temp1));
        }
        else if(rect==false&&rectPainting==true) // 画矩形完毕
        {
            QImage temp2 = *showed; // 用临时变量保存图片以展示
            QPainter painter(&temp2);
            painter.setRenderHints(QPainter::Antialiasing);
            QPen pen(color, 3);
            painter.setPen(pen);
            QRect rectangle(previousPos-bias,currentPos-bias); // 消除偏差后的坐标
            painter.drawRect(rectangle);
            ui->label_picture->setPixmap(QPixmap::fromImage(temp2));
            rectPainting=false;
            rect=true;
            /*---------------------------弹出对话框存储信息------------------------*/
            Labelname* labelname = new Labelname;
            labelname->exec();
            if(labelname->name!="")
            {
                QVector<QVector<LabelInfo>> OldList=LabelInfoList;
                LabelInfoList[currentItem].append(LabelInfo(0,emptyPoly,rectangle,color,labelname->name));

                updateLabelList(currentItem); // 更新标签列表
                updateLabeledPicture(currentItem); // 更新被标记的图片
                hasSaved=false; // 提示保存
                undoStack->push(new DecAddDeleteCommand(&LabelInfoList,OldList,LabelInfoList)); // 存入undo栈
            }
            else
            {
                ui->label_picture->setPixmap(QPixmap::fromImage(*showed));
            }
            delete labelname;
        }
    }
    /*----------------------画多边形---------------------*/
    if(state==1)
    {
        if(poly==true&&polyPainting==true) // 正在画多边形
        {
            QImage temp1 = *showed; // 用临时变量保存图片以展示
            QPainter painter(&temp1);
            painter.setRenderHints(QPainter::Antialiasing);
            QPen pen(color, 3);
            painter.setPen(pen);
            for(int i=0;i<polyRecorder.size();i++)
            {
                if(i<polyRecorder.size()-1) // 固定线复现
                {
                    QLine line(polyRecorder[i]-bias,polyRecorder[i+1]-bias);
                    painter.drawLine(line);
                }
                else //当前移动点
                {
                    QLine line(polyRecorder[i]-bias,currentPos-bias);
                    painter.drawLine(line);
                }

            }
            ui->label_picture->setPixmap(QPixmap::fromImage(temp1));
        }
        else if(poly==false&&polyPainting==true) // 画多边形完毕
        {
            QImage temp2 = *showed; // 用临时变量保存图片以展示
            QPainter painter(&temp2);
            painter.setRenderHints(QPainter::Antialiasing);
            QPen pen(color, 3);
            painter.setPen(pen);
            for(int i=0;i<polyRecorder.size();i++)
            {
                if(i<polyRecorder.size()-1) // 固定线复现
                {
                    QLine line(polyRecorder[i]-bias,polyRecorder[i+1]-bias);
                    painter.drawLine(line);
                }
                else //最后一段直接连接
                {
                    QLine line(polyRecorder[i]-bias,polyRecorder[0]-bias);
                    painter.drawLine(line);
                }
            }

            ui->label_picture->setPixmap(QPixmap::fromImage(temp2));
            polyPainting=false;
            poly=true;

            /*---------------------------弹出对话框存储信息------------------------*/
            Labelname* labelname = new Labelname;
            labelname->exec();
            if(labelname->name!="")
            {
                QVector<QVector<LabelInfo>> OldList=LabelInfoList;
                for(int i=0;i<polyRecorder.size();i++) // 保存前先映射到图片坐标
                {
                    polyRecorder[i]-=bias;
                }
                LabelInfoList[currentItem].append(LabelInfo(1,polyRecorder,emptyRect,color,labelname->name));

                updateLabelList(currentItem); // 更新标签列表
                updateLabeledPicture(currentItem); // 更新被标记的图片
                undoStack->push(new DecAddDeleteCommand(&LabelInfoList,OldList,LabelInfoList)); // 存入undo栈
                hasSaved=false; // 提示保存
            }
            else
            {
                ui->label_picture->setPixmap(QPixmap::fromImage(*showed));
            }
            polyRecorder.clear();
            delete labelname;
        }
    }
    if(state==2)
    {
        if(segState==0||segState==2) // 鼠标没有按下，则在原来的基础上显示画笔
        {
            QImage temp1 = *showed; // 用临时变量保存图片以展示
            QPainter painter(&temp1);
            painter.setRenderHints(QPainter::Antialiasing);
            QColor tranColor(color.red(),color.green(),color.blue(),100);
            QPen pen(tranColor, 1);
            QBrush brush(tranColor);
            painter.setPen(pen);
            painter.setBrush(brush);
            if(penShape==0) // 方形
            {
                QRect rectangle((currentPos-bias).x()-penSize,(currentPos-bias).y()-penSize,2*penSize,2*penSize);
                painter.drawRect(rectangle);
            }
            else if(penShape==1) // 圆形
            {
                painter.drawEllipse((currentPos-bias).x()-penSize,(currentPos-bias).y()-penSize,2*penSize,2*penSize);
            }
            ui->label_picture->setPixmap(QPixmap::fromImage(temp1));
        }
        if(segState==1) // 鼠标按下，在原来的画布上作画
        {
            QPainter painter(showed);
            painter.setRenderHints(QPainter::Antialiasing);
            QPen pen(color, 1);
            QBrush brush(color);
            painter.setPen(pen);
            painter.setBrush(brush);
            if(penShape==0) // 方形
            {
                QRect rectangle((currentPos-bias).x()-penSize,(currentPos-bias).y()-penSize,2*penSize,2*penSize);
                painter.drawRect(rectangle);
            }
            else if(penShape==1) // 圆形
            {
                painter.drawEllipse((currentPos-bias).x()-penSize,(currentPos-bias).y()-penSize,2*penSize,2*penSize);
            }
            ui->label_picture->setPixmap(QPixmap::fromImage(*showed));
        }
    }
}

void Label2D::colorchange(int c)
{
    switch(c)
    {
        case 0:
            color=Qt::red;
            break;
        case 1:
            color=QColor(255,97,0);
            break;
        case 2:
            color=Qt::yellow;
            break;
        case 3:
            color=QColor(0,255,0);
            break;
        case 4:
            color=QColor(200,0,200);
            break;
        case 5:
            color=QColor(0,0,255);
            break;
    }
}

void Label2D::updateLabelList(int item)
{
    ui->labelList->clear();
    if(LabelInfoList.size()<1) // 没有图片直接返回
    {
        return;
    }
    if(state==0||state==1) // 检测标记状态
    {
        if(LabelInfoList[item].size()<1) // 图片没有标签直接返回
        {
            return;
        }
        QVector<LabelInfo> thisInfo = LabelInfoList[item];
        for(int i=0;i<thisInfo.size();i++)
        {
            QString iconPath="";
            if(thisInfo[i].color==Qt::red)
            {
                iconPath=":/icon/color/red.png";
            }
            else if(thisInfo[i].color==QColor(255,97,0))
            {
                iconPath=":/icon/color/orange.png";
            }
            else if(thisInfo[i].color==Qt::yellow)
            {
                iconPath=":/icon/color/yellow.png";
            }
            else if(thisInfo[i].color==QColor(0,255,0))
            {
                iconPath=":/icon/color/green.png";
            }
            else if(thisInfo[i].color==QColor(200,0,200))
            {
                iconPath=":/icon/color/purple.png";
            }
            else if(thisInfo[i].color==QColor(0,0,255))
            {
                iconPath=":/icon/color/blue.png";
            }
            QIcon icon(iconPath);
            QListWidgetItem* item=new QListWidgetItem(icon,thisInfo[i].name);
            ui->labelList->addItem(item);
        }
        /*----------更新信息-------------*/
        if(ui->labelList->count()==0)
        {
            currentLabel=-1;
        }
        else
        {
            ui->labelList->setCurrentRow(0);
            currentLabel=0;
        }
    }
    else if(state==2) // 分割标记状态，从文件夹中读入图片代表标记
    {
        segLabels.clear();
        QString path = InfoList[currentItem].path()+"/"+InfoList[currentItem].baseName(); // 找到存储分割标记的文件夹
        QDir dir(path);
        dir.setFilter(QDir::Files);
        QFileInfoList list = dir.entryInfoList();
        int t=0;
        for(int i=0;i<list.size();i++)
        {
            QFileInfo Info2D(list[i].filePath());
            if(Info2D.suffix()=="jpg"||Info2D.suffix()=="jpeg"||Info2D.suffix()=="bmp" //保证只读入图片
                    ||Info2D.suffix()=="gif"||Info2D.suffix()=="png")
            {
                segLabels.append(Info2D);
                QListWidgetItem* item=new QListWidgetItem(segLabels[t].baseName());
                qDebug()<<segLabels[t].baseName();
                ui->labelList->addItem(item);
                t++;
            }
        }
        if(ui->labelList->count()==0)
        {
            currentLabel=-1;
        }
        else
        {
            ui->labelList->setCurrentRow(0);
            labelChange(0);
            currentLabel=0;
        }
    }
}

void Label2D::updateLabeledPicture(int item)
{
    if(state==2)
    {
        /*------------------------展示原图----------------------*/
        openPicture(InfoList[currentItem]);
        return;
    }
    QImage temp;
    temp = *labeled;
    *showed = temp;
    for(int i=0;i<LabelInfoList[item].size();i++)
    {
        if(LabelInfoList[item][i].type==0) // 矩形
        {
            /*------------绘制矩形---------------*/
            QPainter painter(showed);
            painter.setRenderHints(QPainter::Antialiasing);
            QPen pen(LabelInfoList[item][i].color, 3);
            painter.setPen(pen);
            QRect rectangle(LabelInfoList[item][i].location); // 消除偏差后的坐标
            painter.drawRect(rectangle);
            /*------------标签框---------------*/
            int X=LabelInfoList[item][i].location.left();
            int Y=LabelInfoList[item][i].location.top();
            if(LabelInfoList[item][i].location.right()<X)
                X=LabelInfoList[item][i].location.right();
            if(LabelInfoList[item][i].location.bottom()<Y)
                Y=LabelInfoList[item][i].location.bottom();
            QColor c(255,255,255,100);
            QBrush brush(c);
            painter.setBrush(brush);
            painter.setPen(QPen(QColor(255,255,255,100),1));
            painter.drawRect(X, Y-20, 50, 20);
            /*------------标签---------------*/
            QFont font("Helvetica");
            painter.setFont(font);
            painter.setPen(QPen(Qt::black,1));
            painter.drawText(X, Y-20, 50, 20, 0, LabelInfoList[item][i].name);
        }
        else
        {
            /*------------绘制多边形---------------*/
            QPainter painter(showed);
            painter.setRenderHints(QPainter::Antialiasing);
            QPen pen(LabelInfoList[item][i].color, 3);
            painter.setPen(pen);
            for(int t=0;t<LabelInfoList[item][i].vertexsSets.size();t++)
            {
                if(t<LabelInfoList[item][i].vertexsSets.size()-1) // 固定线复现
                {
                    QLine line(LabelInfoList[item][i].vertexsSets[t],LabelInfoList[item][i].vertexsSets[t+1]);
                    painter.drawLine(line);
                }
                else //最后一段直接连接
                {
                    QLine line(LabelInfoList[item][i].vertexsSets[t],LabelInfoList[item][i].vertexsSets[0]);
                    painter.drawLine(line);
                }
            }
            /*------------标签框和标签---------------*/
            /*------------标签框---------------*/
            int X=LabelInfoList[item][i].vertexsSets[0].x();
            int Y=LabelInfoList[item][i].vertexsSets[0].y();
            QColor c(255,255,255,100);
            QBrush brush(c);
            painter.setBrush(brush);
            painter.setPen(QPen(QColor(255,255,255,100),1));
            painter.drawRect(X-50, Y-20, 50, 20);
            //------------标签---------------
            QFont font("Helvetica");
            painter.setFont(font);
            painter.setPen(QPen(Qt::black,1));
            painter.drawText(X-50, Y-20, 50, 20, 0, LabelInfoList[item][i].name);
        }
    }
    ui->label_picture->setPixmap(QPixmap::fromImage(*showed));
}

void Label2D::gotoNext()
{
    currentItem++;
    if(currentItem>=InfoList.size())
    {
        currentItem--;
        return;
    }
    itemChange(currentItem);
}

void Label2D::gotoPrevious()
{
    currentItem--;
    if(currentItem<0)
    {
        currentItem++;
        return;
    }
    itemChange(currentItem);
}

void Label2D::savelabeled()
{
    if(InfoList.size()==0)
        return;
    //---------------------首先创建一个文件夹----------------------
    QDir *labeledPicture = new QDir;
    bool exist = labeledPicture->exists(InfoList[currentItem].absolutePath()+"/labeledPicture");
    if(!exist)
    {
        labeledPicture->mkdir(InfoList[currentItem].absolutePath()+"/labeledPicture"); // 创建labeledPicture文件夹
    }

    //---------------------将图片保存在文件夹中----------------------
    QFile file(InfoList[currentItem].absolutePath()+"/labeledPicture/"+InfoList[currentItem].fileName());//创建一个文件对象，存储用户选择的文件
    if (!file.open(QIODevice::ReadWrite))
    {
        return;
    }
    //----------------------------以流方式写入文件-------------------------
    QByteArray ba;
    QBuffer buffer(&ba);
    showed->save(&buffer, "JPG");//把图片以流方式写入文件缓存流中
    buffer.open(QIODevice::WriteOnly);
    file.write(ba);//将流中的图片写入文件对象当中

    /*---------------------将标签信息保存在同一个目录中----------------------*/
    QFile textFile(InfoList[currentItem].absolutePath()+"/"+InfoList[currentItem].baseName()+".txt");
    if(textFile.open(QIODevice::WriteOnly|QIODevice::Text))
    {
        QTextStream out(&textFile);
        out<<LabelInfoList[currentItem].size()<<endl; // 第一行是一个整数，表示标签的数量
        for(int i=0;i<LabelInfoList[currentItem].size();i++)
        {
            out<<LabelInfoList[currentItem][i].type<<" "; // 类型
            if(LabelInfoList[currentItem][i].type==0) // 输出矩形标签
            {
                out<<LabelInfoList[currentItem][i].name<<" "; // 名称
                out<<colorToString(LabelInfoList[currentItem][i].color)<<" "; // 颜色
                out<<LabelInfoList[currentItem][i].location.top()<<" ";
                out<<LabelInfoList[currentItem][i].location.left()<<" ";
                out<<LabelInfoList[currentItem][i].location.bottom()<<" ";
                out<<LabelInfoList[currentItem][i].location.right();
                out<<endl;
            }
            else //输出多边形标签
            {
                out<<LabelInfoList[currentItem][i].name<<" "; // 名称
                out<<colorToString(LabelInfoList[currentItem][i].color)<<" "; // 颜色
                out<<LabelInfoList[currentItem][i].vertexsSets.size()<<" ";
                for(int t=0;t<LabelInfoList[currentItem][i].vertexsSets.size();t++)
                {
                    out<<LabelInfoList[currentItem][i].vertexsSets[t].x()<<" ";
                    out<<LabelInfoList[currentItem][i].vertexsSets[t].y()<<" ";
                }
            }
        }

        textFile.close();
    }
}

QString Label2D::colorToString(QColor c)
{
    if(c==Qt::red)
    {
        return "red";
    }
    else if(c==QColor(255,97,0))
    {
        return "orange";
    }
    else if(c==Qt::yellow)
    {
        return "yellow";
    }
    else if(c==QColor(0,255,0))
    {
        return "green";
    }
    else if(c==QColor(200,0,200))
    {
        return "purple";
    }
    else if(c==QColor(0,0,255))
    {
        return "blue";
    }
    else
    {
        return "wrong";
    }
}

QColor Label2D::stringToColor(QString s)
{
    if(s=="red")
    {
        return Qt::red;
    }
    else if(s=="orange")
    {
        return QColor(255,97,0);
    }
    else if(s=="yellow")
    {
        return Qt::yellow;
    }
    else if(s=="green")
    {
        return QColor(0,255,0);
    }
    else if(s=="purple")
    {
        return QColor(200,0,200);
    }
    else if(s=="blue")
    {
        return QColor(0,0,255);
    }
    else
    {
        return Qt::black;
    }
}

void Label2D::closelabeled()
{
    if(LabelInfoList.size()==0) // 空
    {
        return;
    }
    if(hasSaved==false) // 提示保存
    {
        if(QMessageBox::Yes==QMessageBox::question(this,"是否保存","是否保存修改？"))
        {
            savelabeled();
        }
        hasSaved=true;
    }
    LabelInfoList.erase(LabelInfoList.begin()+currentItem);
    InfoList.erase(InfoList.begin()+currentItem);
    /*----------------------重置列表------------------------------*/
    ui->nameList->clear();
    QStringList pictureNames; // 创建向量保存每个图片的名称和完全的路径
    for(int i=0;i<InfoList.size();i++)
    {
        pictureNames.append(InfoList[i].fileName());
    }
    ui->nameList->addItems(pictureNames);


    if(LabelInfoList.size()==0) // 关闭了唯一一张
    {
        ui->currentPicture->setText("");
        ui->currentDir->setText("");
        ui->label_picture->clear();
        updateLabelList(currentItem);
        return;
    }
    else if(currentItem==LabelInfoList.size()) // 关闭了最后一张
    {
        currentItem--;
        openPicture(InfoList[currentItem]); // 还是打开最后一张
        ui->nameList->setCurrentRow(currentItem);
    }
    else
    {
        openPicture(InfoList[currentItem]); // 默认打开下一张
        ui->nameList->setCurrentRow(currentItem);
    }
}

void Label2D::labelChange(int label)
{
    if(label<0) // 避免自动设置为-1导致程序崩溃
        return;
    currentLabel = label;
    ui->labelList->setCurrentRow(currentLabel);
    if(state==2) // 分割状态下，显示相应的图片
    {
        QString path = segLabels[label].path()+"/"+segLabels[label].fileName();
        QImageReader reader(path);
        reader.setDecideFormatFromContent(true);
        QImage* img=new QImage;
        if(!reader.read(img)) // 加载图像失败
        {
            delete img;
            return;
        }
        ui->label_picture->setPixmap(QPixmap::fromImage(*img));
    }
}

void Label2D::deletelabel()
{
    if(InfoList.size()==0)
        return;
    if(state==0||state==1)
    {
        if(LabelInfoList[currentItem].size()==0)
            return;
        QVector<QVector<LabelInfo>> oldList= LabelInfoList;
        LabelInfoList[currentItem].erase(LabelInfoList[currentItem].begin()+currentLabel);
        updateLabelList(currentItem);
        updateLabeledPicture(currentItem);
        if(ui->labelList->count()==0) // 关闭了唯一一张
        {
            currentLabel=-1;
            return;
        }
        else
        {
            ui->labelList->setCurrentRow(0);
            currentLabel=0;
        }
        hasSaved=false; // 提示保存
        undoStack->push(new DecAddDeleteCommand(&LabelInfoList,oldList,LabelInfoList));
    }
    else if(state==2)
    {
        if(segLabels[currentItem].size()==0)
            return;
        QFile labelFile(segLabels[currentLabel].path()+"/"+segLabels[currentLabel].fileName()); // 在文件夹中删除图片
        labelFile.remove();
        QFile textFile(segLabels[currentLabel].path()+"/"+segLabels[currentLabel].baseName()+".txt");
        textFile.remove();
        segLabels.erase(segLabels.begin()+currentLabel); // 在列表中删除条目
        updateLabelList(currentItem);
    }
}

void Label2D::modeChanged(int i)
{
    if(i==1)
    {
        state=2;
        beginSegm();
    }
    else
    {
        ui->buttonRect->pressed();
        ui->buttonRect->setChecked(true);
    }
}

void Label2D::beginSegm()
{
    if(InfoList.size()==0)
        return;
    /*---------------------首先创建一个文件夹----------------------*/
    QDir *segPicture = new QDir;
    bool exist = segPicture->exists(InfoList[currentItem].absolutePath()+"/"+InfoList[currentItem].baseName());
    if(!exist)
    {
        segPicture->mkdir(InfoList[currentItem].absolutePath()+"/"+InfoList[currentItem].baseName()); // 创建labeledPicture文件夹
    }

    /*------------------------展示原图----------------------*/
    openPicture(InfoList[currentItem]);
}

void Label2D::segFinish()
{
    if(segState!=2)//还没有标记点什么完成
    {
        QMessageBox::information(this,"没有可保存的标记","没有可保存的标记");
        return;
    }
    /*---------------------------弹出对话框存储信息------------------------*/
    Labelname* labelname = new Labelname;
    labelname->exec();
    if(labelname->name=="")
    {
        delete labelname;
        return;
    }
    /*---------------------将图片保存在文件夹中----------------------*/
    QFile file(InfoList[currentItem].absolutePath()+"/"+InfoList[currentItem].baseName()+"/"+labelname->name+".jpg");//创建一个文件对象，存储用户选择的文件
    if (!file.open(QIODevice::ReadWrite))
    {
        return;
    }
    /*----------------------------以流方式写入文件-------------------------*/
    QByteArray ba;
    QBuffer buffer(&ba);
    showed->save(&buffer, "JPG");//把图片以流方式写入文件缓存流中
    buffer.open(QIODevice::WriteOnly);
    file.write(ba);//将流中的图片写入文件对象当中

    /*----------------------同时生成一个txt文件，记录所有被标记的像素--------------------*/
    /*---------------------将标签信息保存在同一个目录中----------------------*/
    QFile textFile(InfoList[currentItem].absolutePath()+"/"+InfoList[currentItem].baseName()+"/"+labelname->name+".txt");
    if(textFile.open(QIODevice::WriteOnly|QIODevice::Text))
    {
        QTextStream out(&textFile);
        for(int y=0;y<showed->height();y++)
        {
            for(int x=0;x<showed->width();x++)
            {
                QColor temp=showed->pixel(x,y);
                if(temp==color)
                {
                    out<<"("<<x<<","<<y<<")"<<" ";
                }
            }
        }
    }
    textFile.close();

    updateLabelList(currentItem); // 刷新
    openPicture(InfoList[currentItem]);
    undoStack->clear(); // 清除撤销栈
}

void Label2D::segCanceled()
{
     openPicture(InfoList[currentItem]);
}

void Label2D::unDo()
{
    undoStack->undo();
    if(state==0||state==1)
    {
        updateLabelList(currentItem);
        updateLabeledPicture(currentItem);
    }
    else if(state==2)
    {
        ui->label_picture->setPixmap(QPixmap::fromImage(*showed));
    }
}

void Label2D::reDo()
{
    undoStack->redo();
    if(state==0||state==1)
    {
        updateLabelList(currentItem);
        updateLabeledPicture(currentItem);
    }
    else if(state==2)
    {
         ui->label_picture->setPixmap(QPixmap::fromImage(*showed));
    }
}

