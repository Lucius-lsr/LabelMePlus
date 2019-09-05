#ifndef LABELS_H
#define LABELS_H
#include"coordinate3d.h"
#include <QDialog>

class LabelInfo
{
public:
    int type; // 0代表矩形，1代表多边形
    QVector<QPoint> vertexsSets; // 记录多边形顶点的集合向量
    QRect location; // 矩形位置向量
    QColor color;
    QString name;

public:
    LabelInfo(int t, QVector<QPoint> v,QRect l, QColor c, QString n)
    {
        type=t;
        vertexsSets=v;
        location = l;
        color=c;
        name=n;
    }
};

class LabelInfo3D
{
public:
    int x1;
    int y1;
    int z1;
    int x2;
    int y2;
    int z2;
    QColor color;
    QString name;
public:
    LabelInfo3D(int x10, int y10, int z10, int x20, int y20, int z20, QColor c, QString n)
    {
        x1=x10;
        y1=y10;
        z1=z10;
        x2=x20;
        y2=y20;
        z2=z20;
        color=c;
        name=n;
    }

};

#endif // LABELS_H
