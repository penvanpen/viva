/*
    This file is part of Viva

    Viva is free software: you can redistribute it and/or modify
    it under the terms of the GNU Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Viva is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Public License for more details.

    You should have received a copy of the GNU Public License
    along with Viva. If not, see <http://www.gnu.org/licenses/>.
*/
#include <QtGui>
#include "VivaTreemapFrame.h"
#include "VivaTreemapWidget.h"

VivaTreemapFrame::VivaTreemapFrame (QWidget *parent)
  : QFrame(parent)
{
  setFrameStyle(Plain | NoFrame);
  graphicsView = new VivaTreemapWidget (this);
//  graphicsView->setRenderHint(QPainter::Antialiasing, false);
//  graphicsView->setDragMode(QGraphicsView::RubberBandDrag);
//  graphicsView->setOptimizationFlags(QGraphicsView::DontSavePainterState);
//  graphicsView->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
//  graphicsView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

  QBoxLayout *topLayout = new QBoxLayout (QBoxLayout::TopToBottom, this);
  topLayout->setMargin(0);
  topLayout->addWidget (graphicsView);

  setLayout (topLayout);
}

VivaTreemapFrame::~VivaTreemapFrame (void)
{
}

void VivaTreemapFrame::showEvent ( QShowEvent * event )
{
}

void VivaTreemapFrame::resizeEvent ( QResizeEvent * event )
{
}