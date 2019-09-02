#ifndef LABELS_H
#define LABELS_H

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



#endif // LABELS_H
