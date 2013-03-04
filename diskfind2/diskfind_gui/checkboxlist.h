#ifndef CHECKBOXLIST_H
#define CHECKBOXLIST_H
#include <QtGui>

class CheckBoxList: public QComboBox
{
      Q_OBJECT
public:
      CheckBoxList(QWidget *widget = 0);
      virtual ~CheckBoxList();
      bool eventFilter(QObject *object, QEvent *event);
      virtual void paintEvent(QPaintEvent *);
      void SetDisplayText(QString text);
      QString GetDisplayText() const;
      void contextMenuEvent(QContextMenuEvent *);
private:
      QString m_DisplayText;
private slots:
      void slotSelectedIndexChanged(int);
};

#endif // CHECKBOXLIST_H
