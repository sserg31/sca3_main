#ifndef DEVCOMBOBOX_H
#define DEVCOMBOBOX_H

#include <QComboBox>

class DevComboBox : public QComboBox
{
    Q_OBJECT

public:
    DevComboBox(QWidget *parent = 0);
    virtual ~DevComboBox() { }

    void setDevs(const QStringList &currentDevs);

private:
    QPoint m_lastDevMousePos;
    int m_lastY;

private slots:
    void updateLastDevMousePos();
};

#endif // DEVCOMBOBOX_H
