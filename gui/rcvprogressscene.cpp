#include "rcvprogressscene.h"

RcvProgressScene::RcvProgressScene(QObject *parent) :
    QGraphicsScene(parent),
    i_blockMap(0),i_oneRowBlocksNum(SCENE_WIDTH)
{
    i_blockMap = new QGraphicsRectItem();
    this->addItem(i_blockMap);
}

void RcvProgressScene::arragneToView(QSize newSize)
{
    if( newSize.width() <= 0 ) return;

    uint limit = newSize.width() / BLOCK_edgeLength;
    this->arrangeToOneRowBlocksLimit(limit);
}

void RcvProgressScene::arrangeToOneRowBlocksLimit(uint limit)
{
//    qDebug()<< "RcvProgressScene::arrangeToOneRowBlocksLimit(" << limit << ")";
    if( limit <= 0) return;
    this->i_oneRowBlocksNum = limit;

    QMapIterator<quint64, QGraphicsRectItem*> i(i_haveBlocks);
    while (i.hasNext()) {
        i.next();
        this->eraseBlock(i.key());
        this->drawBlock(i.key());
    }
}

void RcvProgressScene::gotBlock(quint64 i)
{
    if(!i_haveBlocks.contains(i))
    this->drawBlock(i);
}

void RcvProgressScene::lostBlock(quint64 i)
{
    this->eraseBlock(i);
}

void RcvProgressScene::clearAllBlocks()
{
    QMapIterator<quint64, QGraphicsRectItem*> i(i_haveBlocks);
    while (i.hasNext()) {
        i.next();
        this->eraseBlock(i.key());
    }
    i_haveBlocks.clear();
}

QGraphicsRectItem *RcvProgressScene::drawBlock(quint64 i)
{
    dBlock b(i, i_oneRowBlocksNum);
    QRectF r(b.ptl, b.pbr);

    QGraphicsRectItem *rect = new QGraphicsRectItem(r);
    rect->setBrush(QBrush(b.color()));
    rect->setParentItem(i_blockMap);

    if( !i_haveBlocks.contains(i))
        i_haveBlocks.insert(i,rect);
    return rect;
}

void RcvProgressScene::eraseBlock(quint64 i)
{
    if( !i_haveBlocks.contains(i)) return;

    QGraphicsRectItem* rect = i_haveBlocks.value(i);
    if(rect){
        delete rect;
    }
}
