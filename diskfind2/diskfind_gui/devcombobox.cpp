#include "devcombobox.h"

#include <QAbstractItemView>
#include <QMouseEvent>
#include <QApplication>
#include <QDebug>
#include <QScrollBar>

DevComboBox::DevComboBox(QWidget *parent) :
    QComboBox(parent)
{
    m_lastY = 0;

    QAbstractItemView *v = view();
    v->setAutoScroll(false);
    v->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    connect(this, SIGNAL(highlighted(int)), this, SLOT(updateLastDevMousePos()));
}

void DevComboBox::setDevs(const QStringList &currentDevs)
{
    static bool firstFill = true;
    int index = currentIndex();
    QString prevText = currentText();

    QAbstractItemView *v = view();

    if (index != -1 && index < currentDevs.size())
        blockSignals(currentDevs[index] == prevText);

    clear();
    addItems(currentDevs);

    bool needActivate = true;
    int pos = currentDevs.indexOf(prevText);

    if (pos == -1)
        pos = 0;

    else
        needActivate = false;

    setCurrentIndex(pos);

    if (needActivate && !firstFill)
        emit activated(pos);

    firstFill = false;
    blockSignals(false);

    QPoint p = v->mapFromGlobal(QCursor::pos());

    if (!(v->rect().contains(p) && v->indexAt(p).isValid()))
        p = m_lastDevMousePos;

    int currentY = v->verticalScrollBar()->value();
    if (m_lastY != currentY)
        p.ry() += m_lastY - currentY;

    p.setX(view()->width()/2);

    m_lastY = currentY;

    QMouseEvent event(QEvent::MouseMove, p, Qt::NoButton, 0, 0);
    // sending to "qt_scrollarea_viewport" instead of v (QComboBoxListView) for older qt version
    QApplication::sendEvent(v->children().at(0), &event);
}

void DevComboBox::updateLastDevMousePos()
{
    QAbstractItemView *v = view();

    m_lastDevMousePos = v->visualRect(v->currentIndex()).bottomRight();
}
