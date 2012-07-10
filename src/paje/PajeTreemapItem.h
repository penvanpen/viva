#ifndef PAJE_TREEMAP_ITEM_H
#define PAJE_TREEMAP_ITEM_H
#include <QPainter>
#include <QGraphicsItem>
#include "PajeEntity.h"
#include "PajeTreemap.h"
#include "PajeTreemapView.h"

class PajeTreemapView;
class PajeTreemap;

class PajeTreemapItem : public QGraphicsItem
{
protected:
  bool hover;
  PajeTreemap *treemap;
  PajeTreemapView *filter;

public:
  PajeTreemapItem (PajeTreemap *treemap, QGraphicsItem *parent, PajeTreemapView *filter);
  QRectF boundingRect (void) const;
  void paint (QPainter *painter, const QStyleOptionGraphicsItem *options, QWidget *widget);
  void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
  void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
};

#endif