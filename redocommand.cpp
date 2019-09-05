#include "redocommand.h"

#include <QDebug>

DecAddDeleteCommand::DecAddDeleteCommand(QVector<QVector<LabelInfo>>* trueList, QVector<QVector<LabelInfo>> oldList, QVector<QVector<LabelInfo>> newList)
{
    m_trueList = trueList;
    m_newList = newList;
    m_oldList = oldList;
}

void DecAddDeleteCommand::undo()
{
    *m_trueList = m_oldList;
}

void DecAddDeleteCommand::redo()
{
    *m_trueList = m_newList;
}

SegAddDeleteCommand::SegAddDeleteCommand(QImage* trueImage, QImage oldImage, QImage newImage)
{
    m_trueImage = trueImage;
    m_oldImage = oldImage;
    m_newImage = newImage;
}

void SegAddDeleteCommand::undo()
{
    *m_trueImage = m_oldImage;
        qDebug()<<"undo,"<<"set to" << *m_trueImage;
}

void SegAddDeleteCommand::redo()
{
    *m_trueImage = m_newImage;
}

/*----------------------------------3D----------------------------------*/
DecAddDeleteCommand3D::DecAddDeleteCommand3D(QVector<QVector<LabelInfo3D>>* trueList, QVector<QVector<LabelInfo3D>> oldList, QVector<QVector<LabelInfo3D>> newList)
{
    m_trueList = trueList;
    m_newList = newList;
    m_oldList = oldList;
}

void DecAddDeleteCommand3D::undo()
{
    *m_trueList = m_oldList;
}

void DecAddDeleteCommand3D::redo()
{
    *m_trueList = m_newList;
}

SegAddDeleteCommand3D::SegAddDeleteCommand3D(QColor**** trueImage,QColor*** oldImage, QColor*** newImage, int zSize, int pixSize)
{
    m_trueImage = trueImage;
    m_oldImage = oldImage;
    m_newImage = newImage;
    m_zSize = zSize;
    m_pixSize = pixSize;
}

void SegAddDeleteCommand3D::undo()
{
    for(int z=0;z<m_zSize;z++)
        for(int y=0;y<m_pixSize;y++)
            for(int x=0;x<m_pixSize;x++)
            {
                m_trueImage[0][z][y][x]=m_oldImage[z][y][x];
            }
}

void SegAddDeleteCommand3D::redo()
{
    qDebug()<<"redo"<<"1" << *m_trueImage;
    for(int z=0;z<m_zSize;z++)
        for(int y=0;y<m_pixSize;y++)
            for(int x=0;x<m_pixSize;x++)
            {
                m_trueImage[0][z][y][x]=m_newImage[z][y][x];
            }
}

SegAddDeleteCommand3D::~SegAddDeleteCommand3D() // 析构函数要求删除三维向量
{
    if(m_oldImage!=nullptr)
    {
        delete m_oldImage;
        m_oldImage=nullptr;
    }
    if(m_newImage!=nullptr)
    {
        delete m_newImage;
        m_newImage=nullptr;
    }
}
