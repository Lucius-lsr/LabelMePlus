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

#endif // REDOCOMMAND_H
