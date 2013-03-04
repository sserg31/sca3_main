#include "markeritemdelegate.h"
#include "itemdataroles.h"

#include <QObject>
#include <QPainter>
#include <QApplication>
#include <QPalette>
#include <QTextOption>

#include <QDebug>

MarkerItemDelegate::MarkerItemDelegate(bool hasToolTip, QObject* parent)
  : QItemDelegate(parent),
    isElided(false),
    hasToolTip(hasToolTip)
{
}


void MarkerItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  painter->save();

  QStyleOptionViewItemV4 opt(option);
  painter->setFont(opt.font);

  QVariant background = index.data(Qt::BackgroundRole);
  if (!background.isNull())
    painter->fillRect(option.rect, background.value<QColor>());
  else
    painter->fillRect(option.rect, opt.backgroundBrush);

  QString text = index.model()->data(index, Qt::DisplayRole).toString();

  int shift = 0;

  QVariant value = index.data(Qt::CheckStateRole);
  if (value.isValid())
  {
    Qt::CheckState checkState = static_cast<Qt::CheckState>(value.toInt());
    QRect checkRect = check(opt, opt.rect, checkState);
    QItemDelegate::drawCheck(painter, opt, checkRect, checkState);
    shift = checkRect.width();
  }

  QRect resultRowRect(opt.rect.adjusted(shift, 0, 0, 0));

  //------------------------------------------------------

  const QWidget *widget = qstyleoption_cast<const QStyleOptionViewItemV3 *>(&option)->widget;
  QStyle *style = widget ? widget->style() : QApplication::style();
  const int textMargin = style->pixelMetric(QStyle::PM_FocusFrameHMargin, 0, widget) + 1;
  QRect textRect = resultRowRect.adjusted(textMargin, 0, -textMargin, 0); // remove width padding

  const bool wrapText = opt.features & QStyleOptionViewItemV2::WrapText;
  textOption.setWrapMode(wrapText ? QTextOption::WordWrap : QTextOption::ManualWrap);
  textOption.setTextDirection(option.direction);
  textOption.setAlignment(QStyle::visualAlignment(option.direction, option.displayAlignment));
  textLayout.setTextOption(textOption);
  textLayout.setFont(option.font);
  QString replacedString = text;

  const QChar nl = QLatin1Char('\n');
  for (int i = 0; i < replacedString.count(); ++i)
    if (replacedString.at(i) == nl)
      replacedString[i] = QChar::LineSeparator;
  textLayout.setText(replacedString);

  QSizeF textLayoutSize = doTextLayout(textRect.width());

  // if we still overflow even after eliding the text, enable clipping

  if (textRect.width() < textLayoutSize.width() || textRect.height() < textLayoutSize.height())
  {
    isElided = true;
    QString elided;
    int start = 0;
    int end = text.indexOf(QChar::LineSeparator, start);
    if (end == -1)
    {
      elided += option.fontMetrics.elidedText(text, option.textElideMode, textRect.width());
    }
    else
    {
      while (end != -1)
      {
        elided += option.fontMetrics.elidedText(text.mid(start, end - start),
                                                option.textElideMode, textRect.width());
        elided += QChar::LineSeparator;
        start = end + 1;
        end = text.indexOf(QChar::LineSeparator, start);
      }

      //let's add the last line (after the last QChar::LineSeparator)
      elided += option.fontMetrics.elidedText(text.mid(start),
                                              option.textElideMode, textRect.width());
    }
    textLayout.setText(elided);
    textLayoutSize = doTextLayout(textRect.width());
  }
  else
  {
    isElided = false;
  }

  drawMarker(painter, index, text, resultRowRect);

  if (hasToolTip)
  {
    QAbstractItemModel* model = const_cast<QAbstractItemModel*>(index.model());
    if (isElided)
      model->setData(index, text, Qt::ToolTipRole);
    else
      model->setData(index, QVariant(), Qt::ToolTipRole);
  }

  QItemDelegate::drawDisplay(painter, opt, resultRowRect, text);
  QItemDelegate::drawFocus(painter, opt, opt.rect);

  painter->restore();
}


void MarkerItemDelegate::drawMarker(QPainter* painter, const QModelIndex& index, const QString& text,
                                    const QRect& rect) const
{
  if (!text.isEmpty())
  {
    const int textMargin = QApplication::style()->pixelMetric(QStyle::PM_FocusFrameHMargin) + 1;

    QVariant beginHighlightVar = index.data(ItemDataRoles::BeginHighLightRole);

    if (beginHighlightVar.isNull())
        return;

    int beginHighlight = index.data(ItemDataRoles::BeginHighLightRole).toInt();
    int endHighlight = index.data(ItemDataRoles::EndHighLightRole).toInt();

    int searchTermStartPixels = painter->fontMetrics().width(text.left(beginHighlight));
    int searchTermLengthPixels = painter->fontMetrics().width(text.mid(beginHighlight, endHighlight-beginHighlight));
    QRect resultHighlightRect(rect);

    resultHighlightRect.setTop(resultHighlightRect.top() + 1);
    resultHighlightRect.setHeight(resultHighlightRect.height() - 1);
    resultHighlightRect.setLeft(resultHighlightRect.left() + searchTermStartPixels + textMargin - 1); // -1: Cosmetics
    resultHighlightRect.setRight(resultHighlightRect.left() + searchTermLengthPixels + 1); // +1: Cosmetics
    painter->fillRect(resultHighlightRect, QBrush(index.data(ItemDataRoles::HighLightColorRole).value<QColor>()));
  }
}


QSizeF MarkerItemDelegate::doTextLayout(int lineWidth) const
{
  qreal height = 0;
  qreal widthUsed = 0;
  textLayout.beginLayout();
  while (true) {
    QTextLine line = textLayout.createLine();
    if (!line.isValid())
      break;
    line.setLineWidth(lineWidth);
    line.setPosition(QPointF(0, height));
    height += line.height();
    widthUsed = qMax(widthUsed, line.naturalTextWidth());
  }
  textLayout.endLayout();
  return QSizeF(widthUsed, height);
}
