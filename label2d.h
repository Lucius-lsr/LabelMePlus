#ifndef LABEL2D_H
#define LABEL2D_H

#include "labelname.h"
#include "labels.h"
#include <QMainWindow>
#include <QFileInfoList>
#include <QListWidgetItem>
#include <QDebug>
#include <QUndoStack>

namespace Ui {
class Label2D;
}

class Label2D : public QMainWindow
{
    Q_OBJECT

public:
    explicit Label2D(QWidget *parent = nullptr);
    ~Label2D();

private:
    void showImage(QImage* img); // 展示图片
    void setCurrentInfo(QFileInfo); // 获取路径和名称并显示
    void openPicture(QFileInfo info); // 打开图片并显示相关信息
    void beginLabel(QImage* img); // 开始绘图的触发函数
    void beginSegm(); // 进入分割模式
    void updateLabelList(int); // 将标签信息显示在标签信息表中
    void updateLabeledPicture(int); // 将标签信息绘制在图片上
    void paintEvent(QPaintEvent *);//重写窗体重绘事件
    void mousePressEvent(QMouseEvent *);//重写鼠标按下事件
    void mouseReleaseEvent(QMouseEvent *);//重写鼠标释放事件
    void mouseMoveEvent(QMouseEvent *);//重写鼠标移动事件

private slots:
    void openSinglePicture(); // 打开单张图片
    void openPictureGroup(); // 打开图片文件夹
    void itemChange(int); // 变更当前图片
    void colorchange(int); // 变更颜色
    void gotoNext(); // 下一张
    void gotoPrevious(); // 下一张
    void savelabeled(); // 保存
    void closelabeled(); // 关闭当前图片
    void labelChange(int); // 变更选择的标签
    void labelChange(QListWidgetItem* ){labelChange(currentLabel);} // 用于只有一个label的特殊情况
    void deletelabel(); // 删除标注
    QString colorToString(QColor); // 颜色转名称
    QColor stringToColor(QString); // 名称转颜色
    void rectMode(){state=0;updateLabelList(currentItem);updateLabeledPicture(currentItem);} // 转化为矩形标注模式
    void polyMode(){state=1;updateLabelList(currentItem);updateLabeledPicture(currentItem);} // 转化为多边形标注模式
    void modeChanged(int i); // 模式变更
    void setPenSize1(){penSize=3;}
    void setPenSize2(){penSize=5;}
    void setPenSize3(){penSize=10;} // 画笔粗细调节
    void setPenShapeRect(){penShape=0;} // 方形画笔
    void setPenShapeCircle(){penShape=1;} // 圆形画笔
    void segFinish(); // 分割标记完成
    void segCanceled(); // 分割标记取消
    void unDo(); // 撤销
    void reDo(); // 重做
    void magnifying(){mag=!mag;if(InfoList.size()<1) mag=false;} // 放大镜模式

private:
    Ui::Label2D *ui;
    QFileInfoList InfoList;
    int state; // 0代表画矩形，1代表画多边形，2代表分割标注
    //----------记录绘图状态-------------
    bool rect;
    bool rectPainting;
    bool poly;
    bool polyPainting;
    bool polyfinish;

    QImage* labeled;// 当前正在标注的图片
    QImage* showed;// 当前正在展示的图片，包含标签
    QPoint previousPos; // 记录之前的坐标，用于绘图
    QPoint currentPos; // 记录当前前的坐标，用于绘图
    QPoint bias; // 记录图片和窗口的位置偏差
    QColor color; // 记录选取的颜色
    int currentItem; // 第几张图片
    int currentLabel; // 第几个标签
    QVector<QVector<LabelInfo>> LabelInfoList; // 管理标签信息的向量
    QVector<LabelInfo> emptyLabelInfos; // 空信息，用于占空间
    QRect emptyRect; // 空矩形，用来占空间
    QVector<QPoint> emptyPoly; // 空多边形，用来占空间
    QVector<QPoint> polyRecorder; // 画图时存储未画完的多边形
    bool hasSaved; // 提示保存
    int penSize;
    int penShape; // 0代表方形，1代表圆形
    int segState; // 0代表没有开始画，1代表鼠标按下，2代表鼠标已释放但已经开始画
    QFileInfoList segLabels; // 存储分割标记的列表
    QUndoStack *undoStack;
    QImage oldImage; // 用来储存原来的图像
    bool mag; // 放大镜模式

};

#endif // LABEL2D_H
