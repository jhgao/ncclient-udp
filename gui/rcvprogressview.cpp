#include "rcvprogressview.h"

RcvProgressView::RcvProgressView(QWidget *parent):
    QGraphicsView(parent)
{
}


void RcvProgressView::resizeEvent(QResizeEvent *event)
{
    emit sig_resized(event->size());
}
