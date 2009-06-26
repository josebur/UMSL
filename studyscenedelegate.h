#ifndef STUDYSCENEDELEGATE_H
#define STUDYSCENEDELEGATE_H

#include <QItemDelegate>

class StudySceneDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    StudySceneDelegate(QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // STUDYSCENEDELEGATE_H
