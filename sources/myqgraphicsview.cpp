#include "headers/myqgraphicsview.h"

myQGraphicsView::myQGraphicsView(QWidget *parent)
{
    this->setParent(parent);
}

void myQGraphicsView::resizeEvent(QResizeEvent *event)
{
    this->fitInView(this->sceneRect(), Qt::KeepAspectRatio);
}
