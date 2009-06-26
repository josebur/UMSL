#include "studyscenedelegate.h"

#include <QComboBox>
#include <QDebug>
#include <QModelIndex>
#include <QPainter>
#include <QSpinBox>
#include <QItemDelegate>

#include <limits>

const int MAX_INTEGER = std::numeric_limits<int>::max();

StudySceneDelegate::StudySceneDelegate(QObject *parent)
    : QItemDelegate(parent)
{
}

QWidget *StudySceneDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const
{
    if (index.column() == 2) {
        QSpinBox *editor = new QSpinBox(parent);
        editor->setMinimum(0);
        editor->setMaximum(MAX_INTEGER);
        return editor;
    }
    return QItemDelegate::createEditor(parent, option, index);
}

void StudySceneDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if (index.column() == 2) {
        int value = index.model()->data(index, Qt::DisplayRole).toInt();

        QSpinBox *spinBox = static_cast<QSpinBox *>(editor);
        spinBox->setValue(value);
    }
    QItemDelegate::setEditorData(editor, index);
}

void StudySceneDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                      const QModelIndex &index) const
{
    if (index.column() == 2) {
        QSpinBox *spinBox = static_cast<QSpinBox *>(editor);
        spinBox->interpretText();
        int value = spinBox->value();

        model->setData(index, value, Qt::DisplayRole);
    }
    QItemDelegate::setModelData(editor, model, index);
}

void StudySceneDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                               const QModelIndex &index) const
{
    if (index.column() == 3) {
        if (index.data(Qt::DisplayRole).toBool() == true) {
            painter->drawText(option.rect, Qt::AlignCenter, "Polling Scene");
        }
        else if (index.data(Qt::DisplayRole).toBool() == false) {
            painter->drawText(option.rect, Qt::AlignCenter, "Non-Polling Scene");
        }
        else {
            painter->drawText(option.rect, Qt::AlignCenter, "Shouldn't See This");
        }
    }
    else {
        QItemDelegate::paint(painter, option, index);
    }
}
