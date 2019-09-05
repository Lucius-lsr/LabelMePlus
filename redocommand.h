#ifndef REDOCOMMAND_H
#define REDOCOMMAND_H

#include "labels.h"
#include <QUndoCommand>

class AddDeleteCommand : public QUndoCommand
{

};

class DecAddDeleteCommand : public AddDeleteCommand // 检测标记的增添动作
{
private:
    QVector<QVector<LabelInfo>> *m_trueList;
    QVector<QVector<LabelInfo>> m_oldList;
    QVector<QVector<LabelInfo>> m_newList;
public:
    DecAddDeleteCommand(QVector<QVector<LabelInfo>>* trueList, QVector<QVector<LabelInfo>> oldList, QVector<QVector<LabelInfo>> newList);
    void undo() override;
    void redo() override;
};

class SegAddDeleteCommand : public AddDeleteCommand // 分割标记的增添动作，增添每一笔
{
private:
    QImage *m_trueImage;
    QImage m_oldImage;
    QImage m_newImage;

public:
    SegAddDeleteCommand(QImage* trueImage, QImage oldImage, QImage newImage);
    void undo() override;
    void redo() override;
};

class DecAddDeleteCommand3D : public AddDeleteCommand // 检测标记的增添动作
{
private:
    QVector<QVector<LabelInfo3D>> *m_trueList;
    QVector<QVector<LabelInfo3D>> m_oldList;
    QVector<QVector<LabelInfo3D>> m_newList;

public:
    DecAddDeleteCommand3D(QVector<QVector<LabelInfo3D>>* trueList, QVector<QVector<LabelInfo3D>> oldList, QVector<QVector<LabelInfo3D>> newList);
    void undo() override;
    void redo() override;
};

class SegAddDeleteCommand3D : public AddDeleteCommand // 分割标记的增添动作
{
private:
    QColor**** m_trueImage;
    QColor*** m_oldImage;
    QColor*** m_newImage;
    int m_zSize;
    int m_pixSize;

public:
    SegAddDeleteCommand3D(QColor**** trueImage, QColor*** oldImage, QColor*** newImage, int zSize,int pixSize);
    ~SegAddDeleteCommand3D() override;
    void undo() override;
    void redo() override;
};

#endif // REDOCOMMAND_H
