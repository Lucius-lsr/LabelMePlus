#ifndef COORDINATE3D_H
#define COORDINATE3D_H

#include <QString>
#include <QObject>

class Coordinate3D : public QObject
{
    Q_OBJECT

public:
    int m_x;
    int m_y;
    int m_z;
public:
    Coordinate3D(){m_x=0;m_y=0;m_z=0;}
public slots:
    void receiveXChange(int value);
    void receiveYChange(int value);
    void receiveZChange(int value);

signals:
    void publishXChange(int);
    void publishYChange(int);
    void publishZChange(int);
};





#endif // COORDINATE3D_H
