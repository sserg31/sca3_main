#ifndef MARKERITEMDELEGATE_H
#define MARKERITEMDELEGATE_H

#include <QItemDelegate>
#include <QTextLayout>
#include <QTextOption>

class MarkerItemDelegate : public QItemDelegate
{
public:
    MarkerItemDelegate(bool hasToolTip = false, QObject* parent = 0);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
    void drawMarker(QPainter* painter, const QModelIndex& index, const QString& text, const QRect& rect) const;
    QSizeF doTextLayout(int lineWidth) const;

    mutable QTextLayout textLayout;
    mutable QTextOption textOption;
    mutable bool isElided;
    bool hasToolTip;
};

#endif // MARKERITEMDELEGATE_H
