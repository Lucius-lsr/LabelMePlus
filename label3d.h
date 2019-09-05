#ifndef LABEL3D_H
#define LABEL3D_H
#include "coordinate3d.h"
#include "labelname.h"
#include "labels.h"
#include <QMainWindow>
#include <QDir>
#include <QListWidgetItem>
#include <QUndoStack>

namespace Ui {
class Label3D;
}

class Label3D : public QMainWindow
{
    Q_OBJECT

public:
    explicit Label3D(QWidget *parent = nullptr);
    ~Label3D();
private:
    bool ifColored(QString coor, int value); // 判断是否要被涂上标记的颜色
    void load3DImage(QDir); // 加载3D图片
    void load3DImageAsTemp(QDir); // 加载3D图片到临时三位数组，以供展示分割标记
    void update3DImageInfo(); // 刷新3D图片信息列表
    void mouseMoveEvent(QMouseEvent *);//重写鼠标移动事件
    void mousePressEvent(QMouseEvent *);//重写鼠标按下事件
    void mouseReleaseEvent(QMouseEvent *);//重写鼠标释放事件
    void paintEvent(QPaintEvent *);//重写窗体重绘事件
    void updateLabelList(int); // 将标签信息显示在标签信息表中
    QString colorToString(QColor); // 颜色转名称
    QColor stringToColor(QString); // 名称转颜色
    void updateSlider();


protected slots:
    void openSinglePicture(); // 打开单张图片
    void openPictureGroup(); //打开多张图片
    void test();
    void validate3View();
    void colorchange(int c);
    void itemChange(int);
    void gotoNext(); // 下一张
    void gotoPrevious(); // 下一张
    void closeImage(); // 关闭当前图片
    void finishLabel(); // 完成一个检测标记
    void deleteLabel(); // 删除标注
    void saveLabel(); // 保存当前标记
    void labelChange(int); // 变更选择的标签
    void labelChange(QListWidgetItem* ){labelChange(currentLabel);} // 用于只有一个label的特殊情况
    void modeChanged(int); // 检测标注和分割标注之间的切换
    void roughSegFinish(); // 完成粗略分割
    void segModeChanged(int); // 分割标注粗略模式和精细模式之间的切换
    void setPenSize1(){penSize=3;}
    void setPenSize2(){penSize=5;}
    void setPenSize3(){penSize=10;}
    void setPenShapeRect(){penShape=0;}
    void setPenShapeCircle(){penShape=1;}
    void segFinish();
    void segCanceled();
    void unDo();
    void reDo();
    void magnifying(){mag=!mag;if(m_3DInfos.size()<1) mag=false;}



private slots:
    /*-------以下为微调按钮的声明----------------*/
    void on_top_X_labeler1_S_clicked();
    void on_top_X_labeler2_S_clicked();
    void on_top_X_observer_S_clicked();
    void on_top_X_labeler1_L_clicked();
    void on_top_X_labeler2_L_clicked();
    void on_top_X_observer_L_clicked();
    void on_top_Y_labeler1_S_clicked();
    void on_top_Y_labeler2_S_clicked();
    void on_top_Y_observer_S_clicked();
    void on_top_Y_labeler1_L_clicked();
    void on_top_Y_labeler2_L_clicked();
    void on_top_Y_observer_L_clicked();
    void on_front_X_labeler1_S_clicked();
    void on_front_X_labeler2_S_clicked();
    void on_front_X_observer_S_clicked();
    void on_front_X_labeler1_L_clicked();
    void on_front_X_labeler2_L_clicked();
    void on_front_X_observer_L_clicked();
    void on_front_Z_labeler1_L_clicked();
    void on_front_Z_labeler2_L_clicked();
    void on_front_Z_observer_L_clicked();
    void on_front_Z_labeler1_S_clicked();
    void on_front_Z_labeler2_S_clicked();
    void on_front_Z_observer_S_clicked();
    void on_left_Y_labeler1_S_clicked();
    void on_left_Y_labeler2_S_clicked();
    void on_left_Y_observer_S_clicked();
    void on_left_Y_labeler1_L_clicked();
    void on_left_Y_labeler2_L_clicked();
    void on_left_Y_observer_L_clicked();
    void on_left_Z_labeler1_L_clicked();
    void on_left_Z_labeler2_L_clicked();
    void on_left_Z_observer_L_clicked();
    void on_left_Z_labeler1_S_clicked();
    void on_left_Z_labeler2_S_clicked();
    void on_left_Z_observer_S_clicked();

private:
    Ui::Label3D *ui;
    QImage* testImage;
    QColor*** m_Image3D; // 存储三维图像的三维数组
    QColor*** m_Image3DCopy; // 分割标记时修改的三维图像
    int zSize,pixSize;// 高度和长度宽度
    Coordinate3D* observer; // 观察者坐标
    Coordinate3D* labeler1; // 标记1坐标
    Coordinate3D* labeler2; // 标记2坐标
    Coordinate3D* mouse; // 鼠标位置坐标
    QColor m_color; // 0代表无色，既不标记，1-6分别代表6种颜色
    QFileInfoList m_3DInfos; // 存储列表中3D图片信息的列表
    QVector<QVector<LabelInfo3D>> LabelInfoList3D; // 管理标签信息的向量
    QVector<LabelInfo3D> emptyLabelInfo; // 空向量，用来填充
    int currentItem; // 现在显示的图片编号
    int currentLabel; // 现在显示的标记编号
    bool mode; //true表示检测标记，false表示分割标记
    bool segRough; // true表示粗略模式，false表示精细模式
    QPoint mousePos;
    QImage topImage;
    QImage frontImage;
    QImage leftImage;
    int penSize;
    int penShape; // 0代表方形，1代表圆形
    bool rightButtonPressed;
    QFileInfoList segLabels; // 存储分割标记的列表
    bool hasSaved;
    QUndoStack *undoStack;
    bool mag; // 放大镜模式

};

#endif // LABEL3D_H
