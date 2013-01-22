#ifndef RCVPROGRESSSCENE_H
#define RCVPROGRESSSCENE_H

#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QPoint>
#include <QMap>
#include <QDebug>
#include <QTimer>
#include <QSize>

#define SCENE_WIDTH     50
#define BLOCK_edgeLength    5

struct dBlock{
    dBlock(quint64 i, uint rowBlockLimit):
        state(BLOCK_COMPLETE),sn(i),x(0),y(0),edgeLength(BLOCK_edgeLength)
    {
        this->x = sn % rowBlockLimit;
        this->y = sn / rowBlockLimit;

        ptl.setX(x*edgeLength);
        ptl.setY(y*edgeLength);
        pbr.setX(ptl.x() + edgeLength);
        pbr.setY(ptl.y() + edgeLength);
    }

    enum blockState{
        BLOCK_EMPTY,
        BLOCK_FILLING,
        BLOCK_COMPLETE
    };

    quint64 sn;     //serial number
    uint y; //row
    uint x; //colume
    blockState state;
    uint edgeLength;  //display size

    QPointF ptl; //point top-left
    QPointF pbr; //point bottom-right

    //convience
    QColor color(){
        QString c;
        switch(this->state){
        case BLOCK_EMPTY: c = "white"; break;
        case BLOCK_FILLING: c = "yellow"; break;
        case BLOCK_COMPLETE: c= "green"; break;
        default: c= "white"; break;
        }
        return QColor(c);
    }
};

class RcvProgressScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit RcvProgressScene(QObject *parent = 0);

public slots:
    void arragneToView(QSize);
    void arrangeToOneRowBlocksLimit(uint);

    void gotBlock(quint64);
    void lostBlock(quint64);

    void clearAllBlocks();

private:
    QGraphicsRectItem * drawBlock(quint64 i);
    void eraseBlock(quint64 i);
    uint i_oneRowBlocksNum;
    QGraphicsRectItem *i_blockMap;
    QMap<quint64, QGraphicsRectItem *> i_haveBlocks;
};

#endif // RCVPROGRESSSCENE_H
