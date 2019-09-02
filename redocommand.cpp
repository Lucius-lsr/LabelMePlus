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
}

void SegAddDeleteCommand::redo()
{
    *m_trueImage = m_newImage;
}
