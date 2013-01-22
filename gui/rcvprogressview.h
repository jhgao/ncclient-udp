#ifndef RCVPROGRESSVIEW_H
#define RCVPROGRESSVIEW_H

#include <QGraphicsView>
#include <QResizeEvent>
#include <QSize>

class RcvProgressView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit RcvProgressView(QWidget * parent = 0);

signals:
    void sig_resized(QSize);

protected:
    void resizeEvent(QResizeEvent *event);
};

#endif // RCVPROGRESSVIEW_H
