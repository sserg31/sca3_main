#include "checkboxlist.h"
#include "checkboxlistdelegate.h"
#include <QtGui>
#include <QMouseEvent>


CheckBoxList::CheckBoxList(QWidget *widget )
        :QComboBox(widget),m_DisplayText()
{
    setContextMenuPolicy(Qt::CustomContextMenu);
    // set delegate items view
    view()->setItemDelegate(new CheckBoxListDelegate(this));
    // Enable editing on items view
    view()->setEditTriggers(QAbstractItemView::CurrentChanged);
    // set "CheckBoxList::eventFilter" as event filter for items view
    view()->viewport()->installEventFilter(this);
    // it just cool to have it as default ;)
    view()->setAlternatingRowColors(true);
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(slotSelectedIndexChanged(int)));
}

void CheckBoxList::contextMenuEvent(QContextMenuEvent *)
{
    m_DisplayText = "";
}

void CheckBoxList::slotSelectedIndexChanged(int )
{
    setCurrentIndex(-1);
}

CheckBoxList::~CheckBoxList()
{
}

bool CheckBoxList::eventFilter(QObject *object, QEvent *event)
{
    // don't close items view after we release the mouse button
    // by simple eating MouseButtonRelease in viewport of items view
    if(event->type() == QEvent::MouseButtonRelease && object==view()->viewport())
    {
        return true;
    }
    return QComboBox::eventFilter(object,event);
}

void CheckBoxList::paintEvent(QPaintEvent *)
{
    QStylePainter painter(this);
    painter.setPen(palette().color(QPalette::Text));
    // draw the combobox frame, focusrect and selected etc.
    QStyleOptionComboBox opt;
    initStyleOption(&opt);
    // if no display text been set , use "..." as default
    if(m_DisplayText.isNull())
        opt.currentText = "...";
    else
        opt.currentText = m_DisplayText;

    painter.drawComplexControl(QStyle::CC_ComboBox, opt);
    // draw the icon and text
    painter.drawControl(QStyle::CE_ComboBoxLabel, opt);
}

void CheckBoxList::SetDisplayText(QString text)
{
    m_DisplayText = text;
}

QString CheckBoxList::GetDisplayText() const
{
    return m_DisplayText;
}
