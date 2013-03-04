#ifndef CHECKBOXLISTDELEGATE_H
#define CHECKBOXLISTDELEGATE_H

#include <QComboBox>
#include <QCheckBox>
#include <QItemDelegate>
#include <QApplication>

class CheckBoxListDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    CheckBoxListDelegate(QObject *parent)
             : QItemDelegate(parent)
      {
        QComboBox *comboBox = qobject_cast<QComboBox*>(parent);

        connect(this, SIGNAL(activated(int)), comboBox, SIGNAL(activated(int)));
      }

      void paint(QPainter *painter, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const
      {
            //Get item data
            bool value = index.data(Qt::CheckStateRole).toBool();
            QString text = index.data(Qt::DisplayRole).toString();

            // fill style options with item data
            const QStyle *style = QApplication::style();
            QStyleOptionButton opt;
            opt.state |= value ? QStyle::State_On : QStyle::State_Off;
            opt.state |= QStyle::State_Enabled;
            opt.text = text;
            opt.rect = option.rect;
            // draw item data as CheckBox
            style->drawControl(QStyle::CE_CheckBox,&opt,painter);
      }



QWidget *createEditor(QWidget *parent,
             const QStyleOptionViewItem &,
             const QModelIndex &) const
      {
             // create check box as our editor
             QCheckBox *editor = new QCheckBox(parent);
             return editor;
      }

void setEditorData(QWidget *editor, const QModelIndex &index) const
      {
             emit activated(index.row());
             //set editor data
             QCheckBox *myEditor = static_cast<QCheckBox*>(editor);
             myEditor->setText(index.data(Qt::DisplayRole).toString());
             myEditor->setChecked(index.data(Qt::CheckStateRole).toBool());
       }


void setModelData(QWidget *editor, QAbstractItemModel *model,const QModelIndex &index) const
       {
             //get the value from the editor (CheckBox)
             QCheckBox *myEditor = static_cast<QCheckBox*>(editor);
             bool value = myEditor->isChecked();
             //set model data
             QMap<int,QVariant> data = model->itemData(index);
             data.insert(Qt::DisplayRole,myEditor->text());
             data.insert(Qt::CheckStateRole, value);
             model->setItemData(index,data);
       }

void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &) const
{
           editor->setGeometry(option.rect);
}

signals:
    void activated(int pos) const;
};

#endif // CHECKBOXLISTDELEGATE_H
