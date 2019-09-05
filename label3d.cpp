#include "label3d.h"
#include "ui_label3d.h"
#include "labelname.h"

#include <QDir>
#include <QFileDialog>
#include <QImageReader>
#include <QDebug>
#include <QMessageBox>
#include <QPainter>
#include <QBuffer>
#include <QMouseEvent>
#include <QSignalMapper>
#include <QRadioButton>

Label3D::Label3D(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Label3D)
{
    ui->setupUi(this);
    m_color=Qt::white; // 白色代表没有颜色，也就是不显示标记的颜色
    //test();
    /*-----------一些必须的变量初始化------------------*/
    zSize=0;
    pixSize = 261;
    observer = new Coordinate3D;
    labeler1 = new Coordinate3D;
    labeler2 = new Coordinate3D;
    mouse = new Coordinate3D;
    mode = true;
    segRough = true;
    penSize =1;
    penShape=0;
    rightButtonPressed = false;
    hasSaved = true;
    /*-----------坐标对象和相应划动条的连接------------------*/
    connect(ui->top_X_observer,SIGNAL(sliderMoved(int)),observer,SLOT(receiveXChange(int)));
    connect(observer,SIGNAL(publishXChange(int)),ui->front_X_observer,SLOT(setValue(int)));
    connect(ui->front_X_observer,SIGNAL(sliderMoved(int)),observer,SLOT(receiveXChange(int)));
    connect(observer,SIGNAL(publishXChange(int)),ui->top_X_observer,SLOT(setValue(int)));
    connect(ui->top_Y_observer,SIGNAL(sliderMoved(int)),observer,SLOT(receiveYChange(int)));
    connect(observer,SIGNAL(publishYChange(int)),ui->left_Y_observer,SLOT(setValue(int)));
    connect(ui->left_Y_observer,SIGNAL(sliderMoved(int)),observer,SLOT(receiveYChange(int)));
    connect(observer,SIGNAL(publishYChange(int)),ui->top_Y_observer,SLOT(setValue(int)));
    connect(ui->front_Z_observer,SIGNAL(sliderMoved(int)),observer,SLOT(receiveZChange(int)));
    connect(observer,SIGNAL(publishZChange(int)),ui->left_Z_observer,SLOT(setValue(int)));
    connect(ui->left_Z_observer,SIGNAL(sliderMoved(int)),observer,SLOT(receiveZChange(int)));
    connect(observer,SIGNAL(publishZChange(int)),ui->front_Z_observer,SLOT(setValue(int)));

    connect(ui->top_X_labeler1,SIGNAL(sliderMoved(int)),labeler1,SLOT(receiveXChange(int)));
    connect(labeler1,SIGNAL(publishXChange(int)),ui->front_X_labeler1,SLOT(setValue(int)));
    connect(ui->front_X_labeler1,SIGNAL(sliderMoved(int)),labeler1,SLOT(receiveXChange(int)));
    connect(labeler1,SIGNAL(publishXChange(int)),ui->top_X_labeler1,SLOT(setValue(int)));
    connect(ui->top_Y_labeler1,SIGNAL(sliderMoved(int)),labeler1,SLOT(receiveYChange(int)));
    connect(labeler1,SIGNAL(publishYChange(int)),ui->left_Y_labeler1,SLOT(setValue(int)));
    connect(ui->left_Y_labeler1,SIGNAL(sliderMoved(int)),labeler1,SLOT(receiveYChange(int)));
    connect(labeler1,SIGNAL(publishYChange(int)),ui->top_Y_labeler1,SLOT(setValue(int)));
    connect(ui->front_Z_labeler1,SIGNAL(sliderMoved(int)),labeler1,SLOT(receiveZChange(int)));
    connect(labeler1,SIGNAL(publishZChange(int)),ui->left_Z_labeler1,SLOT(setValue(int)));
    connect(ui->left_Z_labeler1,SIGNAL(sliderMoved(int)),labeler1,SLOT(receiveZChange(int)));
    connect(labeler1,SIGNAL(publishZChange(int)),ui->front_Z_labeler1,SLOT(setValue(int)));

    connect(ui->top_X_labeler2,SIGNAL(sliderMoved(int)),labeler2,SLOT(receiveXChange(int)));
    connect(labeler2,SIGNAL(publishXChange(int)),ui->front_X_labeler2,SLOT(setValue(int)));
    connect(ui->front_X_labeler2,SIGNAL(sliderMoved(int)),labeler2,SLOT(receiveXChange(int)));
    connect(labeler2,SIGNAL(publishXChange(int)),ui->top_X_labeler2,SLOT(setValue(int)));
    connect(ui->top_Y_labeler2,SIGNAL(sliderMoved(int)),labeler2,SLOT(receiveYChange(int)));
    connect(labeler2,SIGNAL(publishYChange(int)),ui->left_Y_labeler2,SLOT(setValue(int)));
    connect(ui->left_Y_labeler2,SIGNAL(sliderMoved(int)),labeler2,SLOT(receiveYChange(int)));
    connect(labeler2,SIGNAL(publishYChange(int)),ui->top_Y_labeler2,SLOT(setValue(int)));
    connect(ui->front_Z_labeler2,SIGNAL(sliderMoved(int)),labeler2,SLOT(receiveZChange(int)));
    connect(labeler2,SIGNAL(publishZChange(int)),ui->left_Z_labeler2,SLOT(setValue(int)));
    connect(ui->left_Z_labeler2,SIGNAL(sliderMoved(int)),labeler2,SLOT(receiveZChange(int)));
    connect(labeler2,SIGNAL(publishZChange(int)),ui->front_Z_labeler2,SLOT(setValue(int)));

    /*-----------坐标对象和相应划动条的连接------------------*/
    connect(observer,SIGNAL(publishXChange(int)),this,SLOT(validate3View()));
    connect(observer,SIGNAL(publishYChange(int)),this,SLOT(validate3View()));
    connect(observer,SIGNAL(publishZChange(int)),this,SLOT(validate3View()));
    connect(labeler1,SIGNAL(publishXChange(int)),this,SLOT(validate3View()));
    connect(labeler1,SIGNAL(publishYChange(int)),this,SLOT(validate3View()));
    connect(labeler1,SIGNAL(publishZChange(int)),this,SLOT(validate3View()));
    connect(labeler2,SIGNAL(publishXChange(int)),this,SLOT(validate3View()));
    connect(labeler2,SIGNAL(publishYChange(int)),this,SLOT(validate3View()));
    connect(labeler2,SIGNAL(publishZChange(int)),this,SLOT(validate3View()));

    /*---------------颜色选项------------------*/
    QRadioButton** colorButtons = new QRadioButton*[7];
    colorButtons[0]=ui->radioButton_red;
    colorButtons[1]=ui->radioButton_ora;
    colorButtons[2]=ui->radioButton_yel;
    colorButtons[3]=ui->radioButton_gre;
    colorButtons[4]=ui->radioButton_pur;
    colorButtons[5]=ui->radioButton_blu;
    colorButtons[6]=ui->radioButton_none;
    QSignalMapper* map = new QSignalMapper(this); // 创建信号映射器
    for(int i=0;i<7;i++)
    {
        connect(colorButtons[i],SIGNAL(pressed()),map,SLOT(map())); // 点击打开
        connect(colorButtons[i],SIGNAL(pressed()),this,SLOT(validate3View()));
        map->setMapping(colorButtons[i], i);
    }
    connect(map, SIGNAL(mapped(int)), this, SLOT(colorchange(int)));



    /*----------------------菜单栏和工具栏-------------------------*/
    connect(ui->actionOpen,SIGNAL(triggered()),this,SLOT(openSinglePicture())); // 点击打开
    connect(ui->toolButton_open,SIGNAL(clicked()),this,SLOT(openSinglePicture())); // 打开的工具栏按钮
    connect(ui->actionOpenFiles,SIGNAL(triggered()),this,SLOT(openPictureGroup())); // 点击打开文件夹
    connect(ui->toolButton_openFiles,SIGNAL(clicked()),this,SLOT(openPictureGroup())); // 打开文件夹的工具栏按钮
    connect(ui->actionNext,SIGNAL(triggered()),this,SLOT(gotoNext())); // 下一张的菜单按钮
    connect(ui->actionPrevious,SIGNAL(triggered()),this,SLOT(gotoPrevious())); // 上一张的菜单按钮
    connect(ui->toolButton_next,SIGNAL(clicked()),this,SLOT(gotoNext())); // 下一张的工具栏按钮
    connect(ui->toolButton_previous,SIGNAL(clicked()),this,SLOT(gotoPrevious())); // 上一张的工具栏按钮
    connect(ui->detect_finish,SIGNAL(clicked()),this,SLOT(finishLabel())); // 完成检测标记按钮
    connect(ui->actionSave,SIGNAL(triggered()),this,SLOT(saveLabel())); // 保存的菜单按钮
    connect(ui->toolButton_save,SIGNAL(clicked()),this,SLOT(saveLabel())); // 保存的工具栏按钮
    connect(ui->actionClose,SIGNAL(triggered()),this,SLOT(closeImage())); // 关闭的菜单按钮
    connect(ui->toolButton_close,SIGNAL(clicked()),this,SLOT(closeImage())); // 关闭的工具栏按钮
    connect(ui->actionDeleteLabel,SIGNAL(triggered()),this,SLOT(deleteLabel())); // 删除标记的菜单按钮
    connect(ui->toolButton_closeLabel,SIGNAL(clicked()),this,SLOT(deleteLabel())); // 删除标记的工具栏按钮
    //connect(ui->actionUndo,SIGNAL(triggered()),this,SLOT(unDo()));
    //connect(ui->actionRedo,SIGNAL(triggered()),this,SLOT(reDo()));
    //connect(ui->toolButton_undo,SIGNAL(clicked()),this,SLOT(unDo()));
    //connect(ui->toolButton_redo,SIGNAL(clicked()),this,SLOT(reDo()));*/
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

    /*----------------------状态栏选择图片和标签----------------------*/
    connect(ui->nameList,SIGNAL(currentRowChanged(int)),this,SLOT(itemChange(int))); // 改变当前的图片
    connect(ui->labelList,SIGNAL(currentRowChanged(int)),this,SLOT(labelChange(int))); // 改变选择的标签
    connect(ui->labelList,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(labelChange(QListWidgetItem*)));

    /*----------------------变更标注模式-------------------------*/
    connect(ui->tabWidget,SIGNAL(currentChanged(int)),this,SLOT(modeChanged(int)));
    connect(ui->rough_finish,SIGNAL(clicked()),this,SLOT(roughSegFinish()));
    connect(ui->tabWidget_seg,SIGNAL(currentChanged(int)),this,SLOT(segModeChanged(int)));
    connect(ui->penSize1,SIGNAL(pressed()),this,SLOT(setPenSize1()));
    connect(ui->penSize2,SIGNAL(pressed()),this,SLOT(setPenSize2()));
    connect(ui->penSize3,SIGNAL(pressed()),this,SLOT(setPenSize3()));
    connect(ui->penShapeRect,SIGNAL(pressed()),this,SLOT(setPenShapeRect()));
    connect(ui->penShapeCircle,SIGNAL(pressed()),this,SLOT(setPenShapeCircle()));
    connect(ui->pushButton_finish,SIGNAL(clicked()),this,SLOT(segFinish()));
    connect(ui->pushButton_cancel,SIGNAL(clicked()),this,SLOT(segCanceled()));

    /*-------------------------微调按钮设计---------------------*/
    ui->top_X_labeler1_S->setArrowType(Qt::LeftArrow);
    ui->top_X_labeler2_S->setArrowType(Qt::LeftArrow);
    ui->top_X_observer_S->setArrowType(Qt::LeftArrow);
    ui->front_X_labeler1_S->setArrowType(Qt::LeftArrow);
    ui->front_X_labeler2_S->setArrowType(Qt::LeftArrow);
    ui->front_X_observer_S->setArrowType(Qt::LeftArrow);
    ui->left_Y_labeler1_S->setArrowType(Qt::LeftArrow);
    ui->left_Y_labeler2_S->setArrowType(Qt::LeftArrow);
    ui->left_Y_observer_S->setArrowType(Qt::LeftArrow);

    ui->top_X_labeler1_L->setArrowType(Qt::RightArrow);
    ui->top_X_labeler2_L->setArrowType(Qt::RightArrow);
    ui->top_X_observer_L->setArrowType(Qt::RightArrow);
    ui->front_X_labeler1_L->setArrowType(Qt::RightArrow);
    ui->front_X_labeler2_L->setArrowType(Qt::RightArrow);
    ui->front_X_observer_L->setArrowType(Qt::RightArrow);
    ui->left_Y_labeler1_L->setArrowType(Qt::RightArrow);
    ui->left_Y_labeler2_L->setArrowType(Qt::RightArrow);
    ui->left_Y_observer_L->setArrowType(Qt::RightArrow);

    ui->top_Y_labeler1_S->setArrowType(Qt::UpArrow);
    ui->top_Y_labeler2_S->setArrowType(Qt::UpArrow);
    ui->top_Y_observer_S->setArrowType(Qt::UpArrow);
    ui->front_Z_labeler1_L->setArrowType(Qt::UpArrow);
    ui->front_Z_labeler2_L->setArrowType(Qt::UpArrow);
    ui->front_Z_observer_L->setArrowType(Qt::UpArrow);
    ui->left_Z_labeler1_L->setArrowType(Qt::UpArrow);
    ui->left_Z_labeler2_L->setArrowType(Qt::UpArrow);
    ui->left_Z_observer_L->setArrowType(Qt::UpArrow);

    ui->top_Y_labeler1_L->setArrowType(Qt::DownArrow);
    ui->top_Y_labeler2_L->setArrowType(Qt::DownArrow);
    ui->top_Y_observer_L->setArrowType(Qt::DownArrow);
    ui->front_Z_labeler1_S->setArrowType(Qt::DownArrow);
    ui->front_Z_labeler2_S->setArrowType(Qt::DownArrow);
    ui->front_Z_observer_S->setArrowType(Qt::DownArrow);
    ui->left_Z_labeler1_S->setArrowType(Qt::DownArrow);
    ui->left_Z_labeler2_S->setArrowType(Qt::DownArrow);
    ui->left_Z_observer_S->setArrowType(Qt::DownArrow);
}

Label3D::~Label3D()
{
    delete ui;
    delete observer;
    delete labeler1;
    delete labeler2;
    for(int z=0;z<zSize;z++) // 高度为zSize
    {
        for(int y=0;y<pixSize;y++) // 长宽均为261
        {
            delete m_Image3D[z][y];
        }
        delete m_Image3D[z];
    }
    delete m_Image3D;
}

void Label3D::openSinglePicture() // 打开单张3D图片,实际上是打开一系列的2D图片,完成3D图像的加载并调用一次观察函数
{
    QString path3D = QFileDialog::getExistingDirectory(this,"选择文件夹",".");
    if(path3D=="") // 取消打开
        return;
    m_3DInfos.append(QFileInfo(path3D)); // 加入列表
    LabelInfoList3D.append(emptyLabelInfo); // 同步添加

    currentItem=m_3DInfos.size()-1; // 选中最后一张列表中的3D图片
    // 刷新信息列表
    QDir dir(m_3DInfos[currentItem].filePath()); // 打开当前选中的图片
    load3DImage(dir); // 加载图片

    /*--------------------自动加载标签-------------------*/
    QFile textfile(QFileInfo(path3D).path()+"/"+QFileInfo(path3D).baseName()+".txt");
    if(textfile.open(QIODevice::ReadOnly | QIODevice::Text)) // 如果有标签数据则加载标签数据
    {
        QTextStream in(&textfile);
        int labelNumber;
        in >> labelNumber;
        for(int i=0;i<labelNumber;i++)
        {
            QString name;
            QString colorName;
            int x1,y1,z1,x2,y2,z2;
            in >> name >> colorName >> x1 >>y1 >>z1 >>x2>>y2>>z2;
            QColor thisColor=stringToColor(colorName);
            LabelInfoList3D[currentItem].append(LabelInfo3D(x1,y1,z1,x2,y2,z2,thisColor,name));
        }
    }
    /*----------------------重置列表------------------------------*/
    ui->nameList->clear();
    QStringList pictureNames; // 创建向量保存每个图片的名称和完全的路径
    for(int i=0;i<m_3DInfos.size();i++)
    {
        pictureNames.append(m_3DInfos[i].fileName());
    }
    ui->nameList->addItems(pictureNames);
    ui->nameList->setCurrentRow(currentItem);

    updateLabelList(currentItem);
    validate3View();
}

void Label3D::openPictureGroup()
{
    QString pathGroup = QFileDialog::getExistingDirectory(this,"选择文件夹",".");
    if(pathGroup=="") // 取消打开
        return;
    QDir groupDir(pathGroup);
    groupDir.setFilter(QDir::Dirs|QDir::NoDotAndDotDot);
    m_3DInfos += groupDir.entryInfoList();
    int toAppend = m_3DInfos.size()-LabelInfoList3D.size();
    for(int i=0;i<toAppend;i++) // 保持同步
    {

        // size -toAppend 是第一个添加的图片位置
        LabelInfoList3D.append(emptyLabelInfo);
        QFileInfo path3D = m_3DInfos[m_3DInfos.size() -toAppend+i].filePath();
        /*--------------------自动加载标签-------------------*/
        QFile textfile(path3D.path()+"/"+path3D.baseName()+".txt");
        if(textfile.open(QIODevice::ReadOnly | QIODevice::Text)) // 如果有标签数据则加载标签数据
        {
            QTextStream in(&textfile);
            int labelNumber;
            in >> labelNumber;
            for(int t=0;t<labelNumber;t++)
            {
                QString name;
                QString colorName;
                int x1,y1,z1,x2,y2,z2;
                in >> name >> colorName >> x1 >>y1 >>z1 >>x2>>y2>>z2;
                QColor thisColor=stringToColor(colorName);
                LabelInfoList3D[m_3DInfos.size() -toAppend+i].append(LabelInfo3D(x1,y1,z1,x2,y2,z2,thisColor,name));
            }
        }
    }

    currentItem=m_3DInfos.size()-1; // 选中最后一张列表中的3D图片
    QDir dir(m_3DInfos[currentItem].filePath()); // 打开当前选中的图片
    load3DImage(dir); // 加载图片

    /*----------------------重置列表------------------------------*/
    ui->nameList->clear();
    QStringList pictureNames; // 创建向量保存每个图片的名称和完全的路径
    for(int i=0;i<m_3DInfos.size();i++)
    {
        pictureNames.append(m_3DInfos[i].fileName());
    }
    ui->nameList->addItems(pictureNames);
    ui->nameList->setCurrentRow(currentItem);

    updateLabelList(currentItem);
    validate3View();
}

void Label3D::load3DImage(QDir dir)
{
    /*---------------先清除原来的图片---------------*/
    for(int z=0;z<zSize;z++) // 高度为zSize
    {
        for(int y=0;y<pixSize;y++) // 长宽均为261
        {
            delete m_Image3D[z][y];
            delete m_Image3DCopy[z][y];
        }
        delete m_Image3D[z];
        delete m_Image3DCopy[z];
    }
    /*---------------载入2D图片---------------*/
    dir.setFilter(QDir::Files);
    dir.setSorting(QDir::Time);
    QFileInfoList list = dir.entryInfoList();
    /*-----------------统计有效2D图片数量作为z的高度--------------*/
    zSize=0;
    for (int i = 0; i < list.size(); i++)
    {
        if(list.at(i).suffix()=="jpg"||list.at(i).suffix()=="jpeg"||list.at(i).suffix()=="bmp" //保证只读入图片
                ||list.at(i).suffix()=="gif"||list.at(i).suffix()=="png")
        {
            zSize++;
        }
    }
    if(zSize==0) // 没有检测到图片
    {
        QMessageBox::information(this,"加载图片错误","没有检测到图片");
        return;
    }
     /*----------------创建3D图片---------------*/
    m_Image3D = new QColor** [zSize];
    m_Image3DCopy = new QColor** [zSize];
    {
        for(int z=0;z<zSize;z++) // 高度为height
        {
            m_Image3D[z] = new QColor* [pixSize];
            m_Image3DCopy[z] = new QColor* [pixSize];
            for(int y=0;y<pixSize;y++) // 长宽均为261
            {
                m_Image3D[z][y] = new QColor [pixSize];
                m_Image3DCopy[z][y] = new QColor [pixSize];
            }
        }
    }
    /*-----------------加载2D图片--------------*/
    int z=0;
    for (int i = 0; i < list.size(); i++)
    {
        QFileInfo Info2D = list.at(i);
        if(Info2D.suffix()=="jpg"||Info2D.suffix()=="jpeg"||Info2D.suffix()=="bmp" //保证只读入图片
                ||Info2D.suffix()=="gif"||Info2D.suffix()=="png")
        {
            QString path2D = Info2D.path()+"/"+Info2D.fileName();
            QImageReader reader(path2D);
            reader.setDecideFormatFromContent(true);
            QImage* img=new QImage;
            if(!reader.read(img)) // 加载图像失败
            {
                delete img;
                continue;
            }
            *img=img->scaled(pixSize,pixSize,Qt::IgnoreAspectRatio); // 使用改变比例完全保留的伸缩策略，要求图片尽量为正方形

            for(int y = 0; y<pixSize; y++)
            {
                for(int x = 0; x<pixSize; x++)
                {
                    m_Image3D[z][y][x] = img->pixel(x,y);
                    m_Image3DCopy[z][y][x] = img->pixel(x,y);
                }
            }
            z++;

            delete img;
        }
    }
    /*--------修改滑动条的范围---------*/
    ui->front_Z_observer->setRange(0,zSize-1);
    ui->front_Z_labeler1->setRange(0,zSize-1);
    ui->front_Z_labeler2->setRange(0,zSize-1);
    ui->left_Z_observer->setRange(0,zSize-1);
    ui->left_Z_labeler1->setRange(0,zSize-1);
    ui->left_Z_labeler2->setRange(0,zSize-1);
    /*---------将所有滑动条移动至中间-----------*/
    observer->m_z=zSize/2;
    labeler1->m_z=zSize/2;
    labeler2->m_z=zSize/2;
    observer->m_x=pixSize/2;
    labeler1->m_x=pixSize/2;
    labeler2->m_x=pixSize/2;
    observer->m_y=pixSize/2;
    labeler1->m_y=pixSize/2;
    labeler2->m_y=pixSize/2;

    updateSlider();
    validate3View(); // 刷新图像
    update3DImageInfo(); // 刷新列表信息
}

void Label3D::load3DImageAsTemp(QDir dir)
{
    /*---------------先清除原来的图片---------------*/
    for(int z=0;z<zSize;z++) // 高度为zSize
    {
        for(int y=0;y<pixSize;y++) // 长宽均为261
        {
            delete m_Image3DCopy[z][y];
        }
        delete m_Image3DCopy[z];
    }
    /*---------------载入2D图片---------------*/
    dir.setFilter(QDir::Files);
    dir.setSorting(QDir::Time);
    QFileInfoList list = dir.entryInfoList();
    /*-----------------统计有效2D图片数量作为z的高度--------------*/
    zSize=0;
    for (int i = 0; i < list.size(); i++)
    {
        if(list.at(i).suffix()=="jpg"||list.at(i).suffix()=="jpeg"||list.at(i).suffix()=="bmp" //保证只读入图片
                ||list.at(i).suffix()=="gif"||list.at(i).suffix()=="png")
        {
            zSize++;
        }
    }
    if(zSize==0) // 没有检测到图片
    {
        QMessageBox::information(this,"加载图片错误","没有检测到图片");
        return;
    }
     /*----------------创建3D图片---------------*/
    m_Image3DCopy = new QColor** [zSize];
    {
        for(int z=0;z<zSize;z++) // 高度为height
        {
            m_Image3DCopy[z] = new QColor* [pixSize];
            for(int y=0;y<pixSize;y++) // 长宽均为261
            {
                m_Image3DCopy[z][y] = new QColor [pixSize];
            }
        }
    }
    /*-----------------加载2D图片--------------*/
    int z=0;
    for (int i = 0; i < list.size(); i++)
    {
        QFileInfo Info2D = list.at(i);
        if(Info2D.suffix()=="jpg"||Info2D.suffix()=="jpeg"||Info2D.suffix()=="bmp" //保证只读入图片
                ||Info2D.suffix()=="gif"||Info2D.suffix()=="png")
        {
            QString path2D = Info2D.path()+"/"+Info2D.fileName();
            QImageReader reader(path2D);
            reader.setDecideFormatFromContent(true);
            QImage* img=new QImage;
            if(!reader.read(img)) // 加载图像失败
            {
                delete img;
                continue;
            }
            *img=img->scaled(pixSize,pixSize,Qt::IgnoreAspectRatio); // 使用改变比例完全保留的伸缩策略，要求图片尽量为正方形

            for(int y = 0; y<pixSize; y++)
            {
                for(int x = 0; x<pixSize; x++)
                {
                    m_Image3DCopy[z][y][x] = img->pixel(x,y);
                }
            }
            z++;

            delete img;
        }
    }
    /*--------修改滑动条的范围---------*/
    ui->front_Z_observer->setRange(0,zSize-1);
    ui->front_Z_labeler1->setRange(0,zSize-1);
    ui->front_Z_labeler2->setRange(0,zSize-1);
    ui->left_Z_observer->setRange(0,zSize-1);
    ui->left_Z_labeler1->setRange(0,zSize-1);
    ui->left_Z_labeler2->setRange(0,zSize-1);
    /*---------将所有滑动条移动至中间-----------*/
    observer->m_z=zSize/2;
    labeler1->m_z=zSize/2;
    labeler2->m_z=zSize/2;
    observer->m_x=pixSize/2;
    labeler1->m_x=pixSize/2;
    labeler2->m_x=pixSize/2;
    observer->m_y=pixSize/2;
    labeler1->m_y=pixSize/2;
    labeler2->m_y=pixSize/2;

    updateSlider();
    validate3View(); // 刷新图像
}


void Label3D::update3DImageInfo()
{
    QString file_name, file_path;
    file_name = m_3DInfos[currentItem].fileName(); // 图片名
    file_path = m_3DInfos[currentItem].absolutePath(); // 图片路径
    ui->currentPicture->setText(file_name);
    ui->currentDir->setText(file_path);
}

void Label3D::itemChange(int item)
{
    if(item<0) // 避免自动设置为-1导致程序崩溃
        return;
    if(m_3DInfos.size()<1)
        return;
    if(hasSaved==false) // 提示保存
    {
        if(QMessageBox::Yes==QMessageBox::question(this,"是否保存","是否保存修改？"))
        {
            saveLabel();
        }
        hasSaved=true;
    }
    currentItem = item;
    QDir dir(m_3DInfos[currentItem].filePath()); // 打开当前选中的图片
    load3DImage(dir); // 加载图片
    ui->nameList->setCurrentRow(currentItem);

    updateLabelList(currentItem);
    updateSlider();
}

void Label3D::validate3View() // 观察函数，传入观察者坐标，得到图像
{
    if(zSize==0)
        return;
    int XView=observer->m_x;
    int YView=observer->m_y;
    int ZView=observer->m_z;

    if(mode) // 检测标记状态
    {
        /*-----------俯视图------------*/
        topImage=QImage(pixSize,pixSize,QImage::Format_ARGB32);
        for(int y = 0; y<pixSize; y++)
        {
            for(int x = 0; x<pixSize; x++)
            {
                topImage.setPixelColor(x,y,m_Image3D[ZView][y][x]);
            }
        }

        if(ifColored("z",ZView)&&(m_color!=Qt::white)) // 在范围内且不是观察模式则画标记框
        {
            QPainter painter(&topImage);
            painter.setPen(QPen(m_color,3));
            painter.drawRect(QRect(QPoint(labeler1->m_x,labeler1->m_y),QPoint(labeler2->m_x,labeler2->m_y)));
        }

        // 画两条直线分割图像
        QPainter linePainter1(&topImage);
        linePainter1.setPen(QPen(Qt::black,2,Qt::DotLine));
        linePainter1.drawLine(XView,0,XView,pixSize);
        linePainter1.drawLine(0,YView,pixSize,YView);

        ui->top_view->setPixmap(QPixmap::fromImage(topImage));
        /*-----------正视图------------*/
        frontImage=QImage(pixSize,zSize,QImage::Format_ARGB32);
        for(int z = 0; z<zSize; z++)
        {
            for(int x = 0; x<pixSize; x++)
            {
                frontImage.setPixelColor(x,zSize-1-z,m_Image3D[z][YView][x]);
            }
        }

        if(ifColored("y",YView)&&(m_color!=Qt::white)) // 在范围内且不是观察模式
        {
            QPainter painter(&frontImage);
            painter.setPen(QPen(m_color,3));
            painter.drawRect(QRect(QPoint(labeler1->m_x,zSize-1-labeler1->m_z),QPoint(labeler2->m_x,zSize-1-labeler2->m_z)));
        }

        // 画两条直线分割图像
        QPainter linePainter2(&frontImage);
        linePainter2.setPen(QPen(Qt::black,2,Qt::DotLine));
        linePainter2.drawLine(XView,0,XView,zSize);
        linePainter2.drawLine(0,zSize-1-ZView,pixSize,zSize-1-ZView);

        ui->front_view->setPixmap(QPixmap::fromImage(frontImage));
        /*-----------左视图------------*/
        leftImage=QImage (pixSize,zSize,QImage::Format_ARGB32);
        for(int z = 0; z<zSize; z++)
        {
            for(int y = 0; y<pixSize; y++)
            {
                leftImage.setPixelColor(y,zSize-1-z,m_Image3D[z][y][XView]);
            }
        }

        if(ifColored("x",XView)&&(m_color!=Qt::white)) // 在范围内且不是观察模式
        {
            QPainter painter(&leftImage);
            painter.setPen(QPen(m_color,3));
            painter.drawRect(QRect(QPoint(labeler1->m_y,zSize-1-labeler1->m_z),QPoint(labeler2->m_y,zSize-1-labeler2->m_z)));
        }

        // 画两条直线分割图像
        QPainter linePainter3(&leftImage);
        linePainter3.setPen(QPen(Qt::black,2,Qt::DotLine));
        linePainter3.drawLine(YView,0,YView,zSize);
        linePainter3.drawLine(0,zSize-1-ZView,pixSize,zSize-1-ZView);

        ui->left_view->setPixmap(QPixmap::fromImage(leftImage));
    }
    else if(!mode&&segRough)
    {
        /*-----------俯视图------------*/
        topImage=QImage (pixSize,pixSize,QImage::Format_ARGB32);
        for(int y = 0; y<pixSize; y++)
        {
            for(int x = 0; x<pixSize; x++)
            {
                topImage.setPixelColor(x,y,m_Image3DCopy[ZView][y][x]);
            }
        }

        if(ifColored("z",ZView)&&(m_color!=Qt::white)) // 在范围内且不是观察模式则画填充框
        {
            QPainter painter(&topImage);
            painter.setPen(QPen(m_color,3));
            painter.drawRect(QRect(QPoint(labeler1->m_x,labeler1->m_y),QPoint(labeler2->m_x,labeler2->m_y)));
        }

        // 画两条直线分割图像
        QPainter linePainter1(&topImage);
        linePainter1.setPen(QPen(Qt::black,2,Qt::DotLine));
        linePainter1.drawLine(XView,0,XView,pixSize);
        linePainter1.drawLine(0,YView,pixSize,YView);

        ui->top_view->setPixmap(QPixmap::fromImage(topImage));
        /*-----------正视图------------*/
        frontImage=QImage(pixSize,zSize,QImage::Format_ARGB32);
        for(int z = 0; z<zSize; z++)
        {
            for(int x = 0; x<pixSize; x++)
            {
                frontImage.setPixelColor(x,zSize-1-z,m_Image3DCopy[z][YView][x]);
            }
        }

        if(ifColored("y",YView)&&(m_color!=Qt::white)) // 在范围内且不是观察模式
        {
            QPainter painter(&frontImage);
            painter.setPen(QPen(m_color,3));
            painter.drawRect(QRect(QPoint(labeler1->m_x,zSize-1-labeler1->m_z),QPoint(labeler2->m_x,zSize-1-labeler2->m_z)));
        }

        // 画两条直线分割图像
        QPainter linePainter2(&frontImage);
        linePainter2.setPen(QPen(Qt::black,2,Qt::DotLine));
        linePainter2.drawLine(XView,0,XView,zSize);
        linePainter2.drawLine(0,zSize-1-ZView,pixSize,zSize-1-ZView);

        ui->front_view->setPixmap(QPixmap::fromImage(frontImage));
        /*-----------左视图------------*/
        leftImage=QImage (pixSize,zSize,QImage::Format_ARGB32);
        for(int z = 0; z<zSize; z++)
        {
            for(int y = 0; y<pixSize; y++)
            {
                leftImage.setPixelColor(y,zSize-1-z,m_Image3DCopy[z][y][XView]);
            }
        }

        if(ifColored("x",XView)&&(m_color!=Qt::white)) // 在范围内且不是观察模式
        {
            QPainter painter(&leftImage);
            painter.setPen(QPen(m_color,3));
            painter.drawRect(QRect(QPoint(labeler1->m_y,zSize-1-labeler1->m_z),QPoint(labeler2->m_y,zSize-1-labeler2->m_z)));
        }

        // 画两条直线分割图像
        QPainter linePainter3(&leftImage);
        linePainter3.setPen(QPen(Qt::black,2,Qt::DotLine));
        linePainter3.drawLine(YView,0,YView,zSize);
        linePainter3.drawLine(0,zSize-1-ZView,pixSize,zSize-1-ZView);

        ui->left_view->setPixmap(QPixmap::fromImage(leftImage));
    }
    else if(!mode&&!segRough)// 分割标记状态
    {
        /*-----------俯视图------------*/
        QImage temp_topImage=QImage (pixSize,pixSize,QImage::Format_ARGB32); // 完全一样的temp不记录分割线和填充框
        topImage=QImage (pixSize,pixSize,QImage::Format_ARGB32);
        for(int y = 0; y<pixSize; y++)
        {
            for(int x = 0; x<pixSize; x++)
            {
                topImage.setPixelColor(x,y,m_Image3DCopy[ZView][y][x]);
                temp_topImage.setPixelColor(x,y,m_Image3DCopy[ZView][y][x]);
            }
        }

        // 画两条直线分割图像
        QPainter linePainter1(&temp_topImage);
        linePainter1.setPen(QPen(Qt::black,2,Qt::DotLine));
        linePainter1.drawLine(XView,0,XView,pixSize);
        linePainter1.drawLine(0,YView,pixSize,YView);

        ui->top_view->setPixmap(QPixmap::fromImage(temp_topImage));
        /*-----------正视图------------*/
        QImage temp_frontImage=QImage (pixSize,zSize,QImage::Format_ARGB32); // 完全一样的temp不记录分割线和填充框
        frontImage=QImage(pixSize,zSize,QImage::Format_ARGB32);
        for(int z = 0; z<zSize; z++)
        {
            for(int x = 0; x<pixSize; x++)
            {
                frontImage.setPixelColor(x,zSize-1-z,m_Image3DCopy[z][YView][x]);
                temp_frontImage.setPixelColor(x,zSize-1-z,m_Image3DCopy[z][YView][x]);
            }
        }

        // 画两条直线分割图像
        QPainter linePainter2(&temp_frontImage);
        linePainter2.setPen(QPen(Qt::black,2,Qt::DotLine));
        linePainter2.drawLine(XView,0,XView,zSize);
        linePainter2.drawLine(0,zSize-1-ZView,pixSize,zSize-1-ZView);

        ui->front_view->setPixmap(QPixmap::fromImage(temp_frontImage));
        /*-----------左视图------------*/
        QImage temp_leftImage=QImage (pixSize,zSize,QImage::Format_ARGB32); // 完全一样的temp不记录分割线和填充框
        leftImage=QImage (pixSize,zSize,QImage::Format_ARGB32);
        for(int z = 0; z<zSize; z++)
        {
            for(int y = 0; y<pixSize; y++)
            {
                leftImage.setPixelColor(y,zSize-1-z,m_Image3DCopy[z][y][XView]);
                temp_leftImage.setPixelColor(y,zSize-1-z,m_Image3DCopy[z][y][XView]);
            }
        }

        // 画两条直线分割图像
        QPainter linePainter3(&temp_leftImage);
        linePainter3.setPen(QPen(Qt::black,2,Qt::DotLine));
        linePainter3.drawLine(YView,0,YView,zSize);
        linePainter3.drawLine(0,zSize-1-ZView,pixSize,zSize-1-ZView);

        ui->left_view->setPixmap(QPixmap::fromImage(temp_leftImage));
    }
}

void Label3D::mouseMoveEvent(QMouseEvent *e) // 俯视图：120，110；正视图：120，490；测视图：520，490；大小均为261*261
{
    mousePos=e->pos();

    if(e->pos().x()>120&&e->pos().x()<(120+261)) // 在俯视图范围内
    {
        if(e->pos().y()>110&&e->pos().y()<(110+261))
        {
            mouse->m_x=(e->pos()-QPoint(120,110)).x();
            mouse->m_y=(e->pos()-QPoint(120,110)).y();
            mouse->m_z=observer->m_z;
        }
    }
    if(e->pos().x()>120&&e->pos().x()<(120+261)) // 在正视图范围内
    {
        if(e->pos().y()>490&&e->pos().y()<(490+261))
        {
            mouse->m_x=(e->pos()-QPoint(120,490)).x();
            mouse->m_z=pixSize/2+zSize/2-(e->pos()-QPoint(120,490)).y();
            mouse->m_y=observer->m_y;
        }
    }
    if(e->pos().x()>520&&e->pos().x()<(520+261)) // 在测试图范围内
    {
        if(e->pos().y()>490&&e->pos().y()<(490+261))
        {
            mouse->m_y=(e->pos()-QPoint(520,490)).x();
            mouse->m_z=pixSize/2+zSize/2-(e->pos()-QPoint(520,490)).y();
            mouse->m_x=observer->m_x;
        }
    }
    if(m_3DInfos.size()>0) // 有图片
    {
        ui->coordinateX->setText(QString::number(mouse->m_x));
        ui->coordinateY->setText(QString::number(mouse->m_y));
        if(mouse->m_z<=zSize-1&&mouse->m_z>=0)
        {
            ui->coordinateZ->setText(QString::number(mouse->m_z));
        }
    }
}

void Label3D::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton) // 左键调节观察视图
    {
    if(e->pos().x()>120&&e->pos().x()<(120+261)) // 在俯视图范围内
        if(e->pos().y()>110&&e->pos().y()<(110+261))
        {
            observer->m_x=mouse->m_x;
            observer->m_y=mouse->m_y;
            if(mouse->m_z<=zSize-1&&mouse->m_z>=0)
            {
                observer->m_z=mouse->m_z;
            }
            updateSlider();
            validate3View(); // 刷新图像
        }
    if(e->pos().x()>120&&e->pos().x()<(120+261)) // 在正视图范围内
        if(e->pos().y()>490&&e->pos().y()<(490+261))
        {
            observer->m_x=mouse->m_x;
            observer->m_y=mouse->m_y;
            if(mouse->m_z<=zSize-1&&mouse->m_z>=0)
            {
                observer->m_z=mouse->m_z;
            }
            updateSlider();
            validate3View(); // 刷新图像
        }
    if(e->pos().x()>520&&e->pos().x()<(520+261)) // 在测试图范围内
        if(e->pos().y()>490&&e->pos().y()<(490+261))
        {
            observer->m_x=mouse->m_x;
            observer->m_y=mouse->m_y;
            if(mouse->m_z<=zSize-1&&mouse->m_z>=0)
            {
                observer->m_z=mouse->m_z;
            }
            updateSlider();
            validate3View(); // 刷新图像
        }
    }
    else if(e->button()==Qt::RightButton) // 右键按下进入绘制状态
    {
       rightButtonPressed = true;
    }
}

void Label3D::mouseReleaseEvent(QMouseEvent *e)
{
    if(e->button()==Qt::RightButton)
    {
        rightButtonPressed=false;
    }

    /*-----放下鼠标后，立刻将图像保存到copy中------*/
    if(!mode&&!segRough&&e->button()==Qt::RightButton) // 只相应右键的
    {
        if(zSize==0)
            return;
        int XView=observer->m_x;
        int YView=observer->m_y;
        int ZView=observer->m_z;

        if(mousePos.x()>120&&mousePos.x()<(120+261)) // 在俯视图范围内修改这一层
        {
            qDebug()<<"temporarily save";
            if(mousePos.y()>110&&mousePos.y()<(110+261))
            {
                for(int y=0;y<pixSize;y++)
                {
                    for(int x=0;x<pixSize;x++)
                    {
                        m_Image3DCopy[ZView][y][x]=topImage.pixel(x,y);
                    }
                }
            }
        }
        if(mousePos.x()>120&&mousePos.x()<(120+261)) // 在正视图范围内修改这一层
        {
            if(mousePos.y()>490&&mousePos.y()<(490+261))
            {
                for(int z=0;z<zSize;z++)
                {
                    for(int x=0;x<pixSize;x++)
                    {
                        m_Image3DCopy[z][YView][x]=frontImage.pixel(x,zSize-1-z);
                    }
                }
            }
        }
        if(mousePos.x()>520&&mousePos.x()<(520+261)) // 在左视图范围内修改这一层
        {
            if(mousePos.y()>490&&mousePos.y()<(490+261))
            {
                for(int z=0;z<zSize;z++)
                {
                    for(int y=0;y<pixSize;y++)
                    {
                        m_Image3DCopy[z][y][XView]=leftImage.pixel(y,zSize-1-z);
                    }
                }
            }
        }
    }
}

void Label3D::paintEvent(QPaintEvent *e)//重写窗体重绘事件
{
    if(!mode&&!segRough)
    {
        if(m_color==Qt::white) // 观察模式
            return;
        if(!rightButtonPressed) // 右键没按下
        {
            if(mousePos.x()>120&&mousePos.x()<(120+261)) // 在俯视图范围内
            {
                if(mousePos.y()>110&&mousePos.y()<(110+261))
                {
                    QImage temp1 = topImage; // 用临时变量保存图片以展示
                    QPainter painter(&temp1);
                    painter.setRenderHints(QPainter::Antialiasing);
                    QColor tranColor(m_color.red(),m_color.green(),m_color.blue(),100);
                    QPen pen(tranColor, 1);
                    QBrush brush(tranColor);
                    painter.setPen(pen);
                    painter.setBrush(brush);
                    qDebug()<<"top"<<"pensize"<<penSize<<"penShape"<<penShape;
                    if(penShape==0) // 方形
                    {
                        QRect rectangle(mouse->m_x-penSize,mouse->m_y-penSize,2*penSize,2*penSize);
                        painter.drawRect(rectangle);
                    }
                    else if(penShape==1) // 圆形
                    {
                        painter.drawEllipse(mouse->m_x-penSize,mouse->m_y-penSize,2*penSize,2*penSize);
                    }
                    ui->top_view->setPixmap(QPixmap::fromImage(temp1));
                }
            }
            if(mousePos.x()>120&&mousePos.x()<(120+261)) // 在正视图范围内
            {
                if(mousePos.y()>490&&mousePos.y()<(490+261))
                {
                    QImage temp1 = frontImage; // 用临时变量保存图片以展示
                    QPainter painter(&temp1);
                    painter.setRenderHints(QPainter::Antialiasing);
                    QColor tranColor(m_color.red(),m_color.green(),m_color.blue(),100);
                    QPen pen(tranColor, 1);
                    QBrush brush(tranColor);
                    painter.setPen(pen);
                    painter.setBrush(brush);
                    qDebug()<<"front"<<"pensize"<<penSize<<"penShape"<<penShape;
                    if(penShape==0) // 方形
                    {
                        QRect rectangle(mouse->m_x-penSize,zSize-mouse->m_z-penSize,2*penSize,2*penSize);
                        painter.drawRect(rectangle);
                    }
                    else if(penShape==1) // 圆形
                    {
                        painter.drawEllipse(mouse->m_x-penSize,+zSize-mouse->m_z-penSize,2*penSize,2*penSize);
                    }
                    ui->front_view->setPixmap(QPixmap::fromImage(temp1));
                }
            }
            if(mousePos.x()>520&&mousePos.x()<(520+261)) // 在左视图范围内
            {
                if(mousePos.y()>490&&mousePos.y()<(490+261))
                {
                    QImage temp1 = leftImage; // 用临时变量保存图片以展示
                    QPainter painter(&temp1);
                    painter.setRenderHints(QPainter::Antialiasing);
                    QColor tranColor(m_color.red(),m_color.green(),m_color.blue(),100);
                    QPen pen(tranColor, 1);
                    QBrush brush(tranColor);
                    painter.setPen(pen);
                    painter.setBrush(brush);
                    qDebug()<<"left"<<"pensize"<<penSize<<"penShape"<<penShape;
                    if(penShape==0) // 方形
                    {
                        QRect rectangle(mouse->m_y-penSize,zSize-mouse->m_z-penSize,2*penSize,2*penSize);
                        painter.drawRect(rectangle);
                    }
                    else if(penShape==1) // 圆形
                    {
                        painter.drawEllipse(mouse->m_y-penSize,zSize-mouse->m_z-penSize,2*penSize,2*penSize);
                    }
                    ui->left_view->setPixmap(QPixmap::fromImage(temp1));
                }
            }
        }
        else //按下右键开始做图
        {
            if(mousePos.x()>120&&mousePos.x()<(120+261)) // 在俯视图范围内
            {
                if(mousePos.y()>110&&mousePos.y()<(110+261))
                {
                    QPainter painter(&topImage);
                    painter.setRenderHints(QPainter::Antialiasing);
                    QPen pen(m_color, 1);
                    QBrush brush(m_color);
                    painter.setPen(pen);
                    painter.setBrush(brush);
                    if(penShape==0) // 方形
                    {
                        QRect rectangle(mouse->m_x-penSize,mouse->m_y-penSize,2*penSize,2*penSize);
                        painter.drawRect(rectangle);
                    }
                    else if(penShape==1) // 圆形
                    {
                        painter.drawEllipse(mouse->m_x-penSize,mouse->m_y-penSize,2*penSize,2*penSize);
                    }
                    ui->top_view->setPixmap(QPixmap::fromImage(topImage));
                }
            }
            if(mousePos.x()>120&&mousePos.x()<(120+261)) // 在正视图范围内
            {
                if(mousePos.y()>490&&mousePos.y()<(490+261))
                {
                    QPainter painter(&frontImage);
                    painter.setRenderHints(QPainter::Antialiasing);
                    QPen pen(m_color, 1);
                    QBrush brush(m_color);
                    painter.setPen(pen);
                    painter.setBrush(brush);
                    if(penShape==0) // 方形
                    {
                        QRect rectangle(mouse->m_x-penSize,zSize-mouse->m_z-penSize,2*penSize,2*penSize);
                        painter.drawRect(rectangle);
                    }
                    else if(penShape==1) // 圆形
                    {
                        painter.drawEllipse(mouse->m_x-penSize,+zSize-mouse->m_z-penSize,2*penSize,2*penSize);
                    }
                    ui->front_view->setPixmap(QPixmap::fromImage(frontImage));
                }
            }
            if(mousePos.x()>520&&mousePos.x()<(520+261)) // 在左视图范围内
            {
                if(mousePos.y()>490&&mousePos.y()<(490+261))
                {
                    QPainter painter(&leftImage);
                    painter.setRenderHints(QPainter::Antialiasing);
                    QPen pen(m_color, 1);
                    QBrush brush(m_color);
                    painter.setPen(pen);
                    painter.setBrush(brush);
                    if(penShape==0) // 方形
                    {
                        QRect rectangle(mouse->m_y-penSize,zSize-mouse->m_z-penSize,2*penSize,2*penSize);
                        painter.drawRect(rectangle);
                    }
                    else if(penShape==1) // 圆形
                    {
                        painter.drawEllipse(mouse->m_y-penSize,zSize-mouse->m_z-penSize,2*penSize,2*penSize);
                    }
                    ui->left_view->setPixmap(QPixmap::fromImage(leftImage));
                }
            }
        }

        /*if(segState==1) // 鼠标按下，在原来的画布上作画
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
        }*/
    }
}


void Label3D::test()
{
    qDebug()<<"generate";
    QImage testImage(261,261,QImage::Format_ARGB32);
    for(int x=0;x<261;x++)
        for(int y=0;y<261;y++)
        {
            testImage.setPixelColor(x,y,Qt::white);
        }
    QPainter painter(&testImage);
    QPen pen(Qt::yellow,3);
    QBrush brush(Qt::green);
    painter.setPen(pen);
    painter.setBrush(brush);
    for(int i=0;i<100;i++)
    {
        QImage tempImage=testImage;
        QPainter painter(&tempImage);
        QPen pen(Qt::red,3);
        QBrush brush(Qt::yellow);
        painter.setPen(pen);
        painter.setBrush(brush);
        painter.drawRect(131-i,131-i,2*i,2*i);

        QFile file("/Users/lishengrui/Desktop/3Dtest2/"+QString::number(i)+".jpg");//创建一个文件对象，存储用户选择的文件
        if (!file.open(QIODevice::ReadWrite))
        {
            return;
        }

        QByteArray ba;
        QBuffer buffer(&ba);
        tempImage.save(&buffer, "JPG");//把图片以流方式写入文件缓存流中
        buffer.open(QIODevice::WriteOnly);
        file.write(ba);//将流中的图片写入文件对象当中
    }
}

void Label3D::colorchange(int c)
{
    switch(c)
    {
        case 0:
            m_color=Qt::red;
            break;
        case 1:
            m_color=QColor(255,97,0);
            break;
        case 2:
            m_color=Qt::yellow;
            break;
        case 3:
            m_color=QColor(0,255,0);
            break;
        case 4:
            m_color=QColor(200,0,200);
            break;
        case 5:
            m_color=QColor(0,0,255);
            break;
        case 6:
            m_color=Qt::white;
            break;
    }
}

bool Label3D::ifColored(QString coor, int value)
{
    if(coor=="z") // 俯视图
    {
        if(labeler1->m_z>value&&labeler2->m_z>value)
            return false;
        else if(labeler1->m_z<value&&labeler2->m_z<value)
            return false;
        else
            return true;
    }
    else if(coor=="y") // 俯视图
    {
        if(labeler1->m_y>value&&labeler2->m_y>value)
            return false;
        else if(labeler1->m_y<value&&labeler2->m_y<value)
            return false;
        else
            return true;
    }
    else if(coor=="x") // 俯视图
    {
        if(labeler1->m_x>value&&labeler2->m_x>value)
            return false;
        else if(labeler1->m_x<value&&labeler2->m_x<value)
            return false;
        else
            return true;
    }
    else
    {
        return false;
    }
}

void Label3D::gotoNext()
{
    currentItem++;
    if(currentItem>=m_3DInfos.size())
    {
        currentItem--;
        return;
    }
    itemChange(currentItem);
}

void Label3D::gotoPrevious()
{
    currentItem--;
    if(currentItem<0)
    {
        currentItem++;
        return;
    }
    itemChange(currentItem);
}

void Label3D::closeImage()
{
    if(m_3DInfos.size()==0) // 空
    {
        return;
    }
    if(hasSaved==false) // 提示保存
    {
        if(QMessageBox::Yes==QMessageBox::question(this,"是否保存","是否保存修改？"))
        {
            saveLabel();
        }
        hasSaved=true;
    }
    m_3DInfos.erase(m_3DInfos.begin()+currentItem);
    LabelInfoList3D.erase(LabelInfoList3D.begin()+currentItem); // 同步
    /*----------------------重置列表------------------------------*/
    ui->nameList->clear();
    QStringList pictureNames; // 创建向量保存每个图片的名称和完全的路径
    for(int i=0;i<m_3DInfos.size();i++)
    {
        pictureNames.append(m_3DInfos[i].fileName());
    }
    ui->nameList->addItems(pictureNames);


    if(m_3DInfos.size()==0) // 关闭了唯一一张
    {
        ui->currentPicture->setText("");
        ui->currentDir->setText("");
        ui->top_view->clear();
        ui->front_view->clear();
        ui->left_view->clear();
        //updateLabelList(currentItem);
        return;
    }
    else if(currentItem==m_3DInfos.size()) // 关闭了最后一张
    {
        currentItem--;
        QDir dir(m_3DInfos[currentItem].filePath()); // 依然打开最后一张
        load3DImage(dir); // 加载图片
        ui->nameList->setCurrentRow(currentItem);
    }
    else
    {
        QDir dir(m_3DInfos[currentItem].filePath()); // 打开当前选中的图片
        load3DImage(dir); // 加载图片
        ui->nameList->setCurrentRow(currentItem);
    }

    updateLabelList(currentItem);
}

void Label3D::finishLabel()
{
    if(m_3DInfos.size()==0)
        return;
    if(m_color==Qt::white)
    {
        QMessageBox::information(this,"没有选择标记颜色","没有选择标记颜色");
        return;
    }

    /*---------------------------弹出对话框存储信息------------------------*/
    Labelname* labelname = new Labelname;
    labelname->exec();
    if(labelname->name!="")
    {
        //QVector<QVector<LabelInfo>> OldList=LabelInfoList;
        LabelInfoList3D[currentItem].append(LabelInfo3D(labeler1->m_x,labeler1->m_y,labeler1->m_z,
                                                        labeler2->m_x,labeler2->m_y,labeler2->m_z,m_color,labelname->name));
        hasSaved=false; // 提示保存
        //undoStack->push(new DecAddDeleteCommand(&LabelInfoList,OldList,LabelInfoList)); // 存入undo栈
    }
    delete labelname;
    updateLabelList(currentItem);
}

void Label3D::saveLabel() // 将labelInfolist3D[currentItem]保存的信息输出到电脑中
{
    /*---------------------将标签信息保存在同一个目录中----------------------*/
    QFile textFile(m_3DInfos[currentItem].absolutePath()+"/"+m_3DInfos[currentItem].baseName()+".txt");
    if(textFile.open(QIODevice::WriteOnly|QIODevice::Text))
    {
        QTextStream out(&textFile);
        out<<LabelInfoList3D[currentItem].size()<<endl; // 第一行是一个整数，表示标签的数量
        for(int i=0;i<LabelInfoList3D[currentItem].size();i++)
        {
            out<<LabelInfoList3D[currentItem][i].name<<" "; // 名称
            out<<colorToString(LabelInfoList3D[currentItem][i].color)<<" "; // 颜色
            out<<LabelInfoList3D[currentItem][i].x1<<" ";
            out<<LabelInfoList3D[currentItem][i].y1<<" ";
            out<<LabelInfoList3D[currentItem][i].z1<<" ";
            out<<LabelInfoList3D[currentItem][i].x2<<" ";
            out<<LabelInfoList3D[currentItem][i].y2<<" ";
            out<<LabelInfoList3D[currentItem][i].z2<<" ";
            out<<endl;
        }
    }
    textFile.close();
}

QString Label3D::colorToString(QColor c)
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

QColor Label3D::stringToColor(QString s)
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

void Label3D::deleteLabel()
{
    //QVector<QVector<LabelInfo3D>> oldList= LabelInfoList;
    if(mode)
    {
        LabelInfoList3D[currentItem].erase(LabelInfoList3D[currentItem].begin()+currentLabel);
        //updateLabelList(currentItem);
        //updateLabeledPicture(currentItem);
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
        updateLabelList(currentItem);
        hasSaved=false; // 提示保存
        //undoStack->push(new DecAddDeleteCommand(&LabelInfoList,oldList,LabelInfoList));
    }
    else // 分割模式下删除图片文件夹和txt文件
    {
        QDir labelFile(segLabels[currentLabel].filePath()); // 在文件夹中删除图片
        qDebug()<<segLabels[currentLabel].filePath();
        labelFile.removeRecursively();
        QFile textFile(segLabels[currentLabel].path()+"/"+segLabels[currentLabel].baseName()+".txt");
        textFile.remove();
        segLabels.erase(segLabels.begin()+currentLabel); // 在列表中删除条目
        updateLabelList(currentItem);
    }
}

void Label3D::updateLabelList(int item)
{
    ui->labelList->clear();
    if(LabelInfoList3D.size()<1) // 没有图片直接返回
    {
        return;
    }
    if(mode) // 检测标注模式
    {
        if(LabelInfoList3D[item].size()<1) // 图片没有标签直接返回
        {
            return;
        }
        QVector<LabelInfo3D> thisInfo = LabelInfoList3D[item];
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
    else // 分割模式直接从文件夹中读取信息
    {
        QString path = m_3DInfos[currentItem].filePath()+"/"+"segmentationLabel"; // 找到存储分割标记的文件夹
        QDir dir(path);
        dir.setFilter(QDir::Dirs|QDir::NoDotAndDotDot);
        QFileInfoList list = dir.entryInfoList();
        for(int i=0;i<list.size();i++)
        {
            QFileInfo Info3D(list[i].filePath());
            segLabels.append(Info3D);
            QListWidgetItem* item=new QListWidgetItem(segLabels[i].baseName());
            ui->labelList->addItem(item);
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

void Label3D::labelChange(int label)
{
    if(label<0) // 避免自动设置为-1导致程序崩溃
        return;
    currentLabel = label;
    ui->labelList->setCurrentRow(currentLabel);

    if(mode)
    {
        m_color=LabelInfoList3D[currentItem][label].color;
        labeler1->m_x=LabelInfoList3D[currentItem][label].x1;
        labeler1->m_y=LabelInfoList3D[currentItem][label].y1;
        labeler1->m_z=LabelInfoList3D[currentItem][label].z1;
        labeler2->m_x=LabelInfoList3D[currentItem][label].x2;
        labeler2->m_y=LabelInfoList3D[currentItem][label].y2;
        labeler2->m_z=LabelInfoList3D[currentItem][label].z2;
    }
    else // 分割模式不显示方框
    {
        m_color=Qt::white;
        // 载入图片
        segLabels[currentLabel].baseName();
        QDir dir(segLabels[currentLabel].filePath()); // 打开当前选中的图片
        load3DImageAsTemp(dir); // 加载图片

    }

    updateSlider();
    validate3View(); // 刷新图像

}

void Label3D::updateSlider()
{
    ui->front_Z_observer->setValue(observer->m_z);
    ui->left_Z_observer->setValue(observer->m_z);
    ui->top_X_observer->setValue(observer->m_x);
    ui->front_X_observer->setValue(observer->m_x);
    ui->top_Y_observer->setValue(observer->m_y);
    ui->left_Y_observer->setValue(observer->m_y);

    ui->front_Z_labeler1->setValue(labeler1->m_z);
    ui->left_Z_labeler1->setValue(labeler1->m_z);
    ui->top_X_labeler1->setValue(labeler1->m_x);
    ui->front_X_labeler1->setValue(labeler1->m_x);
    ui->top_Y_labeler1->setValue(labeler1->m_y);
    ui->left_Y_labeler1->setValue(labeler1->m_y);

    ui->front_Z_labeler2->setValue(labeler2->m_z);
    ui->left_Z_labeler2->setValue(labeler2->m_z);
    ui->top_X_labeler2->setValue(labeler2->m_x);
    ui->front_X_labeler2->setValue(labeler2->m_x);
    ui->top_Y_labeler2->setValue(labeler2->m_y);
    ui->left_Y_labeler2->setValue(labeler2->m_y);

    //同时改变颜色按钮
    QString color=colorToString(m_color);
    if(color=="red")
    {
        ui->radioButton_red->click();
    }
    else if(color=="orange")
    {
        ui->radioButton_ora->click();
    }
    else if(color=="yellow")
    {
        ui->radioButton_yel->click();
    }
    else if(color=="green")
    {
        ui->radioButton_gre->click();
    }
    else if(color=="purple")
    {
        ui->radioButton_pur->click();
    }
    else if(color=="blue")
    {
        ui->radioButton_blu->click();
    }
    else {
        ui->radioButton_none->click();
    }
}

void Label3D::modeChanged(int m)
{
    if(m==0)
    {
        mode=true; // 检测标注
    }
    else
    {
        mode=false; // 分割标注
    }
    qDebug()<<"Dection mode"<<mode;
    validate3View();
    updateLabelList(currentItem);
}

void Label3D::segModeChanged(int m)
{
    if(m==0)
    {
        segRough=true; // 检测标注
    }
    else
    {
        segRough=false; // 分割标注
    }
    qDebug()<<"segRough"<<segRough;

}

void Label3D::roughSegFinish() // 完成粗略分割
{
    int z1,z2,y1,y2,x1,x2; //确定着色位置
    if(labeler1->m_z<labeler2->m_z)
    {
        z1=labeler1->m_z;
        z2=labeler2->m_z;
    }
    else {
        z1=labeler2->m_z;
        z2=labeler1->m_z;
    }
    if(labeler1->m_y<labeler2->m_y)
    {
        y1=labeler1->m_y;
        y2=labeler2->m_y;
    }
    else {
        y1=labeler2->m_y;
        y2=labeler1->m_y;
    }
    if(labeler1->m_x<labeler2->m_x)
    {
        x1=labeler1->m_x;
        x2=labeler2->m_x;
    }
    else {
        x1=labeler2->m_x;
        x2=labeler1->m_x;
    }

    for(int z=z1;z<=z2;z++) // 分割着色
        for(int y=y1;y<=y2;y++)
            for(int x=x1;x<=x2;x++)
            {
                m_Image3DCopy[z][y][x]=m_color;
            }

    validate3View();

}

void Label3D::segFinish()
{
    Labelname* labelname = new Labelname;
    labelname->exec();
    if(labelname->name=="")
    {
        delete labelname;
        return;
    }
    /*---------------------首先创建两个文件夹----------------------*/
    QDir *segPicture = new QDir;
    bool exist = segPicture->exists(m_3DInfos[currentItem].absolutePath()+"/"+m_3DInfos[currentItem].baseName()+"/segmentationLabel");
    if(!exist)
    {
        segPicture->mkdir(m_3DInfos[currentItem].absolutePath()+"/"+m_3DInfos[currentItem].baseName()+"/segmentationLabel"); // 创建labeledPicture文件夹
    }
    bool existLabel = segPicture->exists(m_3DInfos[currentItem].absolutePath()+"/"+m_3DInfos[currentItem].baseName()+"/segmentationLabel/"+labelname->name);
    if(!existLabel)
    {
        segPicture->mkdir(m_3DInfos[currentItem].absolutePath()+"/"+m_3DInfos[currentItem].baseName()+"/segmentationLabel/"+labelname->name); // 创建labeledPicture文件夹
    }
    //---------------------将图片保存在文件夹中----------------------
    for(int i=0;i<zSize;i++)
    {
        QFile file(m_3DInfos[currentItem].absolutePath()+"/"+m_3DInfos[currentItem].baseName()+"/segmentationLabel"+"/"
                   +labelname->name+"/"+QString::number(i+1)+".jpg");//创建一个文件对象，存储用户选择的文件
        if (!file.open(QIODevice::ReadWrite))
        {
            return;
        }

        //----------------------------以流方式写入文件-------------------------
        QByteArray ba;
        QBuffer buffer(&ba);
        QImage img(pixSize,pixSize,QImage::Format_ARGB32);
        for(int y = 0; y<pixSize; y++)
        {
            for(int x = 0; x<pixSize; x++)
            {
                img.setPixelColor(x,y,m_Image3DCopy[i][y][x]);
            }
        }
        img.save(&buffer, "JPG");//把图片以流方式写入文件缓存流中
        buffer.open(QIODevice::WriteOnly);
        file.write(ba);//将流中的图片写入文件对象当中
    }

    //----------------------同时生成一个txt文件，记录所有被标记的像素--------------------
    //---------------------将标签信息保存在同一个目录中----------------------
    QFile textFile(m_3DInfos[currentItem].absolutePath()+"/"+m_3DInfos[currentItem].baseName()+"/segmentationLabel/"+labelname->name+".txt");
    if(textFile.open(QIODevice::WriteOnly|QIODevice::Text))
    {
        QTextStream out(&textFile);
        for(int z=0;z<zSize;z++)
        {
            for(int y=0;y<pixSize;y++)
            {
                for(int x=0;x<pixSize;x++)
                {
                    if(m_Image3DCopy[z][y][x]==m_color)
                    {
                        out << "(" << x << "," << y << "," << z <<")"<<" ";
                    }
                }
            }
        }
    }
    textFile.close();

    updateLabelList(currentItem);
}

void Label3D::segCanceled()
{
    for(int z=0;z<zSize;z++)
        for(int y=0;y<pixSize;y++)
            for(int x=0;x<pixSize;x++)
            {
                m_Image3DCopy[z][y][x]=m_Image3D[z][y][x];
            }
    validate3View();
}

void Label3D::on_top_X_labeler1_S_clicked()
{
    labeler1->m_x--;
    if(labeler1->m_x<0)
        labeler1->m_x=0;
    updateSlider();
    validate3View();
}

void Label3D::on_top_X_labeler2_S_clicked()
{
    labeler2->m_x--;
    if(labeler2->m_x<0)
        labeler2->m_x=0;
    updateSlider();
    validate3View();
}

void Label3D::on_top_X_observer_S_clicked()
{
    observer->m_x--;
    if(observer->m_x<0)
        observer->m_x=0;
    updateSlider();
    validate3View();
}

void Label3D::on_top_X_labeler1_L_clicked()
{
    labeler1->m_x++;
    if(labeler1->m_x>=pixSize)
        labeler1->m_x=pixSize-1;
    updateSlider();
    validate3View();
}

void Label3D::on_top_X_labeler2_L_clicked()
{
    labeler2->m_x++;
    if(labeler2->m_x>=pixSize)
        labeler2->m_x=pixSize-1;
    updateSlider();
    validate3View();
}

void Label3D::on_top_X_observer_L_clicked()
{
    observer->m_x++;
    if(observer->m_x>=pixSize)
        observer->m_x=pixSize-1;
    updateSlider();
    validate3View();
}

void Label3D::on_top_Y_labeler1_S_clicked()
{
    labeler1->m_y--;
    if(labeler1->m_y<0)
        labeler1->m_y=0;
    updateSlider();
    validate3View();
}

void Label3D::on_top_Y_labeler2_S_clicked()
{
    labeler2->m_y--;
    if(labeler2->m_y<0)
        labeler2->m_y=0;
    updateSlider();
    validate3View();
}

void Label3D::on_top_Y_observer_S_clicked()
{
    observer->m_y--;
    if(observer->m_y<0)
        observer->m_y=0;
    updateSlider();
    validate3View();
}

void Label3D::on_top_Y_labeler1_L_clicked()
{
    labeler1->m_y++;
    if(labeler1->m_y>=pixSize)
        labeler1->m_y=pixSize-1;
    updateSlider();
    validate3View();
}

void Label3D::on_top_Y_labeler2_L_clicked()
{
    labeler2->m_y++;
    if(labeler2->m_y>=pixSize)
        labeler2->m_y=pixSize-1;
    updateSlider();
    validate3View();
}

void Label3D::on_top_Y_observer_L_clicked()
{
    observer->m_y++;
    if(observer->m_y>=pixSize)
        observer->m_y=pixSize-1;
    updateSlider();
    validate3View();
}

void Label3D::on_front_X_labeler1_S_clicked()
{
    labeler1->m_x--;
    if(labeler1->m_x<0)
        labeler1->m_x=0;
    updateSlider();
    validate3View();
}

void Label3D::on_front_X_labeler2_S_clicked()
{
    labeler2->m_x--;
    if(labeler2->m_x<0)
        labeler2->m_x=0;
    updateSlider();
    validate3View();
}

void Label3D::on_front_X_observer_S_clicked()
{
    observer->m_x--;
    if(observer->m_x<0)
        observer->m_x=0;
    updateSlider();
    validate3View();
}

void Label3D::on_front_X_labeler1_L_clicked()
{
    labeler1->m_x++;
    if(labeler1->m_x>=pixSize)
        labeler1->m_x=pixSize-1;
    updateSlider();
    validate3View();
}

void Label3D::on_front_X_labeler2_L_clicked()
{
    labeler2->m_x++;
    if(labeler2->m_x>=pixSize)
        labeler2->m_x=pixSize-1;
    updateSlider();
    validate3View();
}

void Label3D::on_front_X_observer_L_clicked()
{
    observer->m_x++;
    if(observer->m_x>=pixSize)
        observer->m_x=pixSize-1;
    updateSlider();
    validate3View();
}

void Label3D::on_front_Z_labeler1_S_clicked()
{
    labeler1->m_z--;
    if(labeler1->m_z<0)
        labeler1->m_z=0;
    updateSlider();
    validate3View();
}

void Label3D::on_front_Z_labeler2_S_clicked()
{
    labeler2->m_z--;
    if(labeler2->m_z<0)
        labeler2->m_z=0;
    updateSlider();
    validate3View();
}

void Label3D::on_front_Z_observer_S_clicked()
{
    observer->m_z--;
    if(observer->m_z<0)
        observer->m_z=0;
    updateSlider();
    validate3View();
}

void Label3D::on_front_Z_labeler1_L_clicked()
{
    labeler1->m_z++;
    if(labeler1->m_z>=pixSize)
        labeler1->m_z=pixSize;
    updateSlider();
    validate3View();
}

void Label3D::on_front_Z_labeler2_L_clicked()
{
    labeler2->m_z++;
    if(labeler2->m_z>=pixSize)
        labeler2->m_z=pixSize;
    updateSlider();
    validate3View();
}

void Label3D::on_front_Z_observer_L_clicked()
{
    observer->m_z++;
    if(observer->m_z>=pixSize)
        observer->m_z=pixSize;
    updateSlider();
    validate3View();
}

void Label3D::on_left_Y_labeler1_S_clicked()
{
    labeler1->m_y--;
    if(labeler1->m_y<0)
        labeler1->m_y=0;
    updateSlider();
    validate3View();
}

void Label3D::on_left_Y_labeler2_S_clicked()
{
    labeler2->m_y--;
    if(labeler2->m_y<0)
        labeler2->m_y=0;
    updateSlider();
    validate3View();
}

void Label3D::on_left_Y_observer_S_clicked()
{
    observer->m_y--;
    if(observer->m_y<0)
        observer->m_y=0;
    updateSlider();
    validate3View();
}

void Label3D::on_left_Y_labeler1_L_clicked()
{
    labeler1->m_y++;
    if(labeler1->m_y>=pixSize)
        labeler1->m_y=pixSize-1;
    updateSlider();
    validate3View();
}

void Label3D::on_left_Y_labeler2_L_clicked()
{
    labeler2->m_y++;
    if(labeler2->m_y>=pixSize)
        labeler2->m_y=pixSize-1;
    updateSlider();
    validate3View();
}

void Label3D::on_left_Y_observer_L_clicked()
{
    observer->m_y++;
    if(observer->m_y>=pixSize)
        observer->m_y=pixSize-1;
    updateSlider();
    validate3View();
}

void Label3D::on_left_Z_labeler1_S_clicked()
{
    labeler1->m_z--;
    if(labeler1->m_z<0)
        labeler1->m_z=0;
    updateSlider();
    validate3View();
}

void Label3D::on_left_Z_labeler2_S_clicked()
{
    labeler2->m_z--;
    if(labeler2->m_z<0)
        labeler2->m_z=0;
    updateSlider();
    validate3View();
}

void Label3D::on_left_Z_observer_S_clicked()
{
    observer->m_z--;
    if(observer->m_z<0)
        observer->m_z=0;
    updateSlider();
    validate3View();
}

void Label3D::on_left_Z_labeler1_L_clicked()
{
    labeler1->m_z++;
    if(labeler1->m_z>=pixSize)
        labeler1->m_z=pixSize;
    updateSlider();
    validate3View();
}

void Label3D::on_left_Z_labeler2_L_clicked()
{
    labeler2->m_z++;
    if(labeler2->m_z>=pixSize)
        labeler2->m_z=pixSize;
    updateSlider();
    validate3View();
}

void Label3D::on_left_Z_observer_L_clicked()
{
    observer->m_z++;
    if(observer->m_z>=pixSize)
        observer->m_z=pixSize;
    updateSlider();
    validate3View();
}
