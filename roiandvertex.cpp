#include "roiandvertex.h"
#include <iostream>  
#include <iomanip> // 对于std::setprecision 
ROIAndVertex::ROIAndVertex(QWidget *parent)
    : QLabel{parent}
{
    connect(this, &ROIAndVertex::signalPaintList, AppEventMGT::getInstance(), &AppEventMGT::trfROIList);

    this->setFocusPolicy(Qt::StrongFocus);
    hasEmit.insert(1, QVector<bool>(100));
    hasEmit.insert(0, QVector<bool>(100));
    this->createMenu();
}

ROIAndVertex::ROIAndVertex()
{

}

void ROIAndVertex::createMenu(){
    //创建下拉菜单
    menu = new QMenu(this);
    actDele = menu->addAction(tr("删除"));
    actCopy = menu->addAction(tr("复制"));
    actPaste = menu->addAction(tr("粘贴"));
    connect(menu, &QMenu::triggered, this, &ROIAndVertex::slotMenuClicked);
}

void ROIAndVertex::slotMenuClicked(QAction* action){
    if(action==actDele){
        if (activePaintEle->paintType == ELLIPSE || activePaintEle->paintType == RECTANGLE) {
            emit signalDelEle(activePaintEle->index);
        }
        else if (activePaintEle->paintType == ANGLE || activePaintEle->paintType == PIPE){
            //ANGLE
            //PIPE
            emit signalDelPipeAngle(activePaintEle->index, activePaintEle->paintType);
        }
        paintList.removeAt(activePaintEle->index2);

    }else if(action==actCopy){
        copyPaintEle(copyPaintEleTopLeft);
    }else if(action==actPaste){
        pastePaintEle(pastePaintEleTopLeft);
    }
    update();
    emit signalPaintList(paintList);
}

void ROIAndVertex::copyPaintEle(QPoint point){
    copyEle = check_pos(point);
}

void ROIAndVertex::pastePaintEle(QPoint point){
    paintElement *copyedEle = new paintElement();  //复制体
    if(copyEle->paintType==ELLIPSE || copyEle->paintType==RECTANGLE){
        QRect rect(point.x(), point.y(), copyEle->rect.width(), copyEle->rect.height());
        copyedEle = new paintElement(rect, copyEle->paintType);

    }else if(copyEle->paintType==LINE){
        QPointF center = copyEle->line.center();
        QPointF p1(point.x()-center.x()+copyEle->line.x1(), point.y()-center.y()+copyEle->line.y1());
        QPointF p2(point.x()-center.x()+copyEle->line.x2(), point.y()-center.y()+copyEle->line.y2());
        copyedEle = new paintElement(QLineF(p1, p2), copyEle->paintType);

    }else if(copyEle->paintType==ANGLE){
        //qDebug()<<"测量的角度还没有被复制";
    }else if(copyEle->paintType==POLYGON){
        QPolygonF polygonF = copyEle->polygon.translated(point-copyEle->polygon[0]);
        copyedEle = new paintElement(polygonF, copyEle->paintType);

    }
    //三角形，管径，管角就不复制粘贴
//    else if(copyEle->paintType==PIPE || copyEle->paintType==ANGLE){
//        copyedEle = new paintElement(copyEle->anglePoints, copyEle->paintType);
//    }
    if(copyedEle->paintType!=NO_TYPE)
        paintList.append(copyedEle);
}

void ROIAndVertex::setDrawingType(PAINT_TYPE type){
    this->drawingType = type;
}

paintElement* ROIAndVertex::check_pos(QPoint point){
    for(int i=0; i<paintList.size(); i++) {
        paintElement *ele = paintList.at(i);
        if(ele->paintType==RECTANGLE || ele->paintType==ELLIPSE){
            if(ele->rect.contains(point)){
                ele->index2 = i;
                this->resetStatFlag(ele, i);
                return ele;
            }
        }else if(ele->paintType==LINE){
            QLineF line = ele->line;
            qreal dis = QLineF(line.p1(), point.toPointF()).length()+QLineF(line.p2(), point.toPointF()).length()-line.length();
            if(dis<=0.5){
                this->resetStatFlag(ele, i);
                return ele;
            }
        }else if(ele->paintType==POLYGON){
            QPolygonF polygonF = ele->polygon;
            for(int i=0; i<polygonF.size(); i++){
                QPointF p = polygonF[i];
                QRectF rectF = QRectF(p.x()-10, p.y()-10, 20, 20);
                if(rectF.contains(point)){
                    this->setCursor(Qt::SizeAllCursor);
                    ele->index2 = i;
                    this->resetStatFlag(ele, i);
                    return ele;
                }
            }
            if(polygonF.containsPoint(point,  Qt::WindingFill)){
                ele->index2 = i;
                this->resetStatFlag(ele, i);
                return ele;
            }
        }else{
            //判断某一点是否存在于三点，两点中
            if(ele->paintType==ANGLE){
                QPolygonF q = QPolygonF(ele->anglePoints);
                if(q.containsPoint(point, Qt::WindingFill)){
                    ele->index2 = i;
                    return ele;
                }
            }else if(ele->paintType==PIPE){
                QPointF p1 = ele->anglePoints.at(0);
                QPointF p2 = ele->anglePoints.at(1);
                QLineF line = QLineF(p1, p2);
                double s = std::abs(double(line.x2()-line.x1())*(point.y()-line.y1())-(line.y2()-line.y1())*(point.x()-line.x1()))/2;
                if(s/line.length()<=3){
                    ele->index2 = i;
                    return ele;
                }
            }
        }
    }

    this->resetStatFlag(this->nullPaintEle, -1);
    return nullPaintEle;
}

HNS::EmDirection ROIAndVertex::rectRegion(QRectF rect, QPointF point){
    int mouseX = point.x();
    int mouseY = point.y();
    QPointF roiTopLeft = rect.topLeft();
    QPointF roiBottomRight = rect.bottomRight();

    HNS::EmDirection dir = HNS::DIR_NONE;
    if (mouseX <= roiTopLeft.x() + CORPADDING && mouseX >= roiTopLeft.x() && mouseY <= roiTopLeft.y() + CORPADDING && mouseY >= roiTopLeft.y())
    {   //左上角
        this->setCursor(Qt::SizeFDiagCursor);
        dir = HNS::DIR_LEFTTOP;
    }
    else if (mouseX >= roiBottomRight.x() - CORPADDING && mouseX < roiBottomRight.x() && mouseY <= roiTopLeft.y() + CORPADDING && mouseY >= roiTopLeft.y())
    {   //右上角
        this->setCursor(Qt::SizeBDiagCursor);
        dir = HNS::DIR_RIGHTTOP;
    }
    else if (mouseX <= roiTopLeft.x() + CORPADDING && mouseX >= roiTopLeft.x() && mouseY >= roiBottomRight.y() - CORPADDING && mouseY <= roiBottomRight.y())
    {   //左下角
        this->setCursor(Qt::SizeBDiagCursor);
        dir = HNS::DIR_LEFTBOTTOM;
    }
    else if (mouseX >= roiBottomRight.x() - CORPADDING && mouseX < roiBottomRight.x() && mouseY >= roiBottomRight.y() - CORPADDING && mouseY <= roiBottomRight.y())
    {   //右下角
        this->setCursor(Qt::SizeFDiagCursor);
        dir = HNS::DIR_RIGHTBOTTOM;
    }
    else if (mouseX >= roiBottomRight.x() - EDGPADDING && mouseX <= roiBottomRight.x() && mouseY >= roiTopLeft.y() && mouseY <= roiBottomRight.y())
    {   //右边
        this->setCursor(Qt::SizeHorCursor);
        dir = HNS::DIR_RIGHT;
    }
    else if (mouseY <= roiTopLeft.y() + EDGPADDING && mouseY >= roiTopLeft.y() && mouseX >= roiTopLeft.x() && mouseX <= roiBottomRight.x())
    {   //上边
        this->setCursor(Qt::SizeVerCursor);
        dir = HNS::DIR_TOP;
    }
    else if (mouseY >= roiBottomRight.y() - EDGPADDING && mouseY <= roiBottomRight.y() && mouseX >= roiTopLeft.x() && mouseX <= roiBottomRight.x())
    {   //下边
        this->setCursor(Qt::SizeVerCursor);
        dir = HNS::DIR_BOTTOM;
    }
    else if (mouseX <= roiTopLeft.x() + EDGPADDING && mouseX >= roiTopLeft.x() && mouseY >= roiTopLeft.y() && mouseY <= roiBottomRight.y())
    {   //左边
        this->setCursor(Qt::SizeHorCursor);
        dir = HNS::DIR_LEFT;
    }
    else if(rect.contains(point))    //中间
    {
        dir = HNS::DIR_MIDDLE;
    }
    else
    {
        dir = HNS::DIR_NONE;
    }
    return dir;
}

HNS::EmDirection ROIAndVertex::regionLine(QLineF line, QPointF point){
    int mouseX = point.x();
    int mouseY = point.y();
    QPointF p1 = line.p1();
    QPointF p2 = line.p2();
    HNS::EmDirection dir = HNS::DIR_NONE;

    //mouseX <= roiTopLeft.x() + CORPADDING && mouseX >= roiTopLeft.x() && mouseY <= roiTopLeft.y() + CORPADDING && mouseY >= roiTopLeft.y()
    if(mouseX <= p1.x()+2 && mouseX >= p1.x()-2 && mouseY <= p1.y()+2 && mouseY >= p1.y()-2){
        this->setCursor(Qt::SizeAllCursor);
        dir = HNS::DIR_FREE;
    }else if(mouseX <= p2.x()+2 && mouseX >= p2.x()-2 && mouseY <= p2.y()+2 && mouseY >= p2.y()-2){
        this->setCursor(Qt::SizeAllCursor);
        dir = HNS::DIR_FREE;
    }else{
        qreal dis = QLineF(line.p1(), point).length()+QLineF(line.p2(), point).length()-line.length();
        if(dis<=0.5){
            dir = HNS::DIR_MIDDLE;
        }
    }
    return dir;
}

HNS::EmDirection ROIAndVertex::regionPolygon(paintElement *ele, QPointF point){
    HNS::EmDirection dir = HNS::DIR_NONE;
    QPolygonF polygonF = ele->polygon;

    for(int i=0; i<polygonF.size(); i++){
        QPointF p = polygonF[i];
        QRectF rectF = QRectF(p.x()-10, p.y()-10, 20, 20);
        if(rectF.contains(point)){
            this->setCursor(Qt::SizeAllCursor);
            return HNS::DIR_FREE;
        }
    }
    if(polygonF.containsPoint(point,  Qt::WindingFill)){
        dir = HNS::DIR_MIDDLE;
    }
    return dir;
}

HNS::EmDirection ROIAndVertex::region(QPointF point){
    for(paintElement* ele: this->paintList){
        //找到激活的ROI
        if(ele==activePaintEle){
            if(ele->paintType==RECTANGLE || ele->paintType==ELLIPSE){
                return this->rectRegion(ele->rect, point);
            }else if(ele->paintType==LINE){
                return this->regionLine(ele->line, point);
            }else if(ele->paintType==POLYGON){
                return this->regionPolygon(ele, point);
            }
        }
    }
    return HNS::DIR_NONE;
}

void ROIAndVertex::mousePressEvent(QMouseEvent *event){
    if(event->button()==Qt::LeftButton){
        activePaintEle = check_pos(event->pos());
        HNS::EmDirection dir = this->region(event->pos());
        qDebug()<<dir;
        if(dir == HNS::DIR_MIDDLE){
            this->setCursor(Qt::ClosedHandCursor);
            this->dragStartPoint = event->pos();
            this->isDragging = true;
            this->isPainting = false;
            this->isScaling = false;
        }else if(dir == HNS::DIR_NONE){  //ROI不存在
            this->setCursor(Qt::ArrowCursor);
            this->isPainting = true;
            this->isDragging = false;
            this->isScaling = false;
            this->paintStartPoint = event->pos();

            if(drawingType==RECTANGLE || drawingType==ELLIPSE)  paintingRect.setTopLeft(paintStartPoint);
            else if(drawingType==LINE)  paintingLine.setP1(paintStartPoint);
            else if(drawingType==POLYGON){
                isPaintingPolygon = true;
                paintingVertex.append(event->pos());
            }else if(drawingType==DISTANCE){
                paintingLine.setP1(paintStartPoint);
            }
        }else{  //鼠标在ROI矩阵的边缘
            this->isScaling = true;
            this->isPainting = false;
            this->isDragging = false;
            this->emCurDir = dir;
        }
    }else if(event->button()==Qt::RightButton){
        if(isPaintingPolygon){
            QPolygonF polygon(paintingVertex);
            paintElement* paintEle = new paintElement(polygon, drawingType);
            paintList.append(paintEle);
            paintingVertex.clear();
            isPainting = false;
            drawingType = NO_TYPE;
            isPaintingPolygon = false;
        }else{
            copyPaintEleTopLeft = event->pos();
            if(copyEle->paintType!=NO_TYPE){
                actPaste->setEnabled(true);
                int x = this->mapFromGlobal(QCursor().pos()).x();
                int y = this->mapFromGlobal(QCursor().pos()).y();
                pastePaintEleTopLeft = QPoint(x,y);
            }else{
                actPaste->setEnabled(false);
            }
            activePaintEle = check_pos(event->pos());
            if(activePaintEle->paintType!=NO_TYPE){
                actDele->setEnabled(true);
                actCopy->setEnabled(true);
            }else{
                actDele->setEnabled(false);
                actCopy->setEnabled(false);
            }
            menu->exec(QCursor::pos());
        }
    }
}

void ROIAndVertex::mouseMoveEvent(QMouseEvent *event){
    if(isPainting){
        paint(event->pos());
    }else if(isDragging){
        drag(event->pos());
    }else if(isScaling){
        scale(activePaintEle, event->pos());
    }
    update();
    emit signalPaintList(paintList);
}

void ROIAndVertex::mouseReleaseEvent(QMouseEvent *event){
    if(isPainting){
        paintElement* paintEle = NULL;
        if(paintingRect.width()>=20 && paintingRect.height()>=20){ //正在绘制矩形
            paintEle = new paintElement(paintingRect, drawingType);
            paintList.append(paintEle);
            isPainting = false;
            drawingType = NO_TYPE;    //绘制完一个ROI之后，需要再次选择绘制的ROI类型才能继续绘制ROI

        }else if(paintingLine.x2()>=0 && paintingLine.length()>=20){
            //qDebug()<<paintingLine.x1()<<","<<paintingLine.y1()<<","<<paintingLine.x2()<<","<<paintingLine.y2();
            paintEle = new paintElement(paintingLine, drawingType);
            paintList.append(paintEle);
            isPainting = false;
            drawingType = NO_TYPE;    //绘制完一个ROI之后，需要再次选择绘制的ROI类型才能继续绘制ROI

        }else if(!paintingVertex.isEmpty()){
            if(drawingType==PIPE && paintingVertex.size()==2){
                paintEle = new paintElement(paintingVertex, drawingType);
                paintList.append(paintEle);
                paintingVertex.clear();
                isPainting = false;
                drawingType = NO_TYPE;    //绘制完一个ROI之后，需要再次选择绘制的ROI类型才能继续绘制ROI

        }else if(drawingType==ANGLE && paintingVertex.size()==3){
                paintEle = new paintElement(paintingVertex, drawingType);
                paintList.append(paintEle);
                paintingVertex.clear();
                isPainting = false;
                drawingType = NO_TYPE;    //绘制完一个ROI之后，需要再次选择绘制的ROI类型才能继续绘制ROI
            }
        }
        //将正在绘制的矩形，椭圆，直线置为原始状态
        paintingLine = QLineF(-1,-1,-1,-1);
        paintingRect = QRect(-1,-1,-1,-1);
        paintStartPoint = QPoint();
        //drawingType = NO_TYPE;    //绘制完一个ROI之后，需要再次选择绘制的ROI类型才能继续绘制ROI

    }else if(isDragging){
        //拖拽操作
        isDragging = false;
        setCursor(Qt::ArrowCursor);
        dragStartPoint = QPoint();
    }else if(isScaling){
        this->setCursor(Qt::ArrowCursor);
        isScaling = false;
    }
    update();
    emit signalPaintList(paintList);
}

void ROIAndVertex::paint(QPoint mousePoint){
    if(drawingType==RECTANGLE || drawingType==ELLIPSE){
        int width = mousePoint.x()-paintStartPoint.x();
        int height = mousePoint.y()-paintStartPoint.y();
        if (width < 0 && height < 0) {//鼠标向左上运动
            if(mousePoint.x()<0 && mousePoint.y()<0){
                paintingRect.setTopLeft(QPoint(0, 0));
                width = paintStartPoint.x();
                height = paintStartPoint.y();
            }else if(mousePoint.x()<0){
                paintingRect.setTopLeft(QPoint(0, mousePoint.y()));
                width = paintStartPoint.x();
            }else if(mousePoint.y()<0){
                paintingRect.setTopLeft(QPoint(mousePoint.x(), 0));
                height = paintStartPoint.y();
            }else{
                paintingRect.setTopLeft(mousePoint);
            }
        } else if(width<0) {//鼠标向左下运动
            if(mousePoint.x()<0 && mousePoint.y()>MUL_LABEL_HEIGHT){
                paintingRect.setTopLeft(QPoint(0, paintStartPoint.y()));
                width = paintStartPoint.x();
                height = MUL_LABEL_HEIGHT-paintStartPoint.y();
            }else if(mousePoint.x()<0){
                paintingRect.setTopLeft(QPoint(0, paintStartPoint.y()));
                width = paintStartPoint.x();
            }else if(mousePoint.y()>MUL_LABEL_HEIGHT){
                height = MUL_LABEL_HEIGHT-paintStartPoint.y();
                paintingRect.setTopLeft(QPoint(mousePoint.x(), paintStartPoint.y()));
            }else{
                paintingRect.setTopLeft(QPoint(mousePoint.x(), paintStartPoint.y()));
            }
        } else if(height < 0){//鼠标向右上运动
            if(mousePoint.x()>MUL_LABEL_WIDTH && mousePoint.y()<0){
                paintingRect.setTopLeft(QPoint(paintStartPoint.x(), 0));
                width = MUL_LABEL_WIDTH-paintStartPoint.x();
                height = paintStartPoint.y();
            }else if(mousePoint.x()>MUL_LABEL_WIDTH){
                paintingRect.setTopLeft(QPoint(paintStartPoint.x(), mousePoint.y()));
                width = MUL_LABEL_WIDTH-paintStartPoint.x();
            }else if(mousePoint.y()<0){
                paintingRect.setTopLeft(QPoint(paintStartPoint.x(), 0));
                height = paintStartPoint.y();
            }else{
                paintingRect.setTopLeft(QPoint(paintStartPoint.x(), mousePoint.y()));
            }
        }else{//鼠标向右下移动
            paintingRect.setTopLeft(paintStartPoint);
            if(mousePoint.x()>MUL_LABEL_WIDTH && mousePoint.y()>MUL_LABEL_HEIGHT){
                width = MUL_LABEL_WIDTH-paintStartPoint.x();
                height = MUL_LABEL_HEIGHT-paintStartPoint.y();
            }else if(mousePoint.x()>MUL_LABEL_WIDTH){
                width = MUL_LABEL_WIDTH-paintStartPoint.x();
            }else if(mousePoint.y()>MUL_LABEL_HEIGHT){
                height = MUL_LABEL_HEIGHT-paintStartPoint.y();
            }
        }
        paintingRect.setSize(QSize(abs(width), abs(height)));
    }else if(drawingType==LINE){
        QPoint p = QPoint(mousePoint);
        if(mousePoint.x()<=0)   p.setX(0);
        else if(mousePoint.x()>=MUL_LABEL_WIDTH) p.setX(MUL_LABEL_WIDTH);
        if(mousePoint.y()<=0)   p.setY(0);
        else if(mousePoint.y()>=MUL_LABEL_HEIGHT)   p.setY(MUL_LABEL_HEIGHT);
        paintingLine.setP2(p);
    }else if(drawingType==DISTANCE){
        paintingLine.setP2(mousePoint);
    }
}

void ROIAndVertex::drag(QPoint mousePoint){
    if(activePaintEle->paintType==ELLIPSE || activePaintEle->paintType==RECTANGLE){
        int dx = mousePoint.x()-dragStartPoint.x();
        int dy = mousePoint.y()-dragStartPoint.y();

        int x = activePaintEle->rect.x()+dx;
        int y = activePaintEle->rect.y()+dy;

        if(x<0) x=0;
        if(y<0) y=0;
        if(x>MUL_LABEL_WIDTH-activePaintEle->rect.width())   x= MUL_LABEL_WIDTH-activePaintEle->rect.width();
        if(y>MUL_LABEL_HEIGHT-activePaintEle->rect.height())  y= MUL_LABEL_HEIGHT-activePaintEle->rect.height();

        QRectF rect;
        rect.setTopLeft(QPoint(x, y));
        rect.setSize(activePaintEle->rect.size());
        activePaintEle->rect = rect;
        dragStartPoint = mousePoint;
    }else if(activePaintEle->paintType==LINE){
        int dx = mousePoint.x()-dragStartPoint.x();
        int dy = mousePoint.y()-dragStartPoint.y();
        QLineF line = activePaintEle->line;

        int x1 = line.x1()+dx;
        int y1 = line.y1()+dy;
        int x2 = line.x2()+dx;
        int y2 = line.y2()+dy;
        if(x1>=0&&x2>=0&&y1>=0&&y2>=0&&x1<=MUL_LABEL_WIDTH&&x2<=MUL_LABEL_WIDTH&&y1<=MUL_LABEL_HEIGHT&&y2<=MUL_LABEL_HEIGHT){
            QPointF p1 = QPointF(x1, y1);
            QPointF p2 = QPointF(x2, y2);
            activePaintEle->line = QLineF(p1, p2);
        }
        dragStartPoint = mousePoint;
    }else if(activePaintEle->paintType==POLYGON){
        int dx = mousePoint.x()-dragStartPoint.x();
        int dy = mousePoint.y()-dragStartPoint.y();

        QRectF bRect = activePaintEle->polygon.boundingRect();
        int x = bRect.x()+dx;
        int y = bRect.y()+dy;

        if(x<0 || x>MUL_LABEL_WIDTH-bRect.width()) dx=0;
        if(y<0 || y>MUL_LABEL_HEIGHT-bRect.height()) dy=0;

        activePaintEle->polygon.translate(dx, dy);
        dragStartPoint = mousePoint;
    }

}

void ROIAndVertex::scale(paintElement* ele, QPoint mousePoint){
    if(ele->paintType==RECTANGLE||ele->paintType==ELLIPSE){
        this->scaleRect(ele, mousePoint);
    }else if(ele->paintType==LINE){
        this->scaleLine(ele, mousePoint);
    }else if(ele->paintType==POLYGON){
        this->scalePolygon(ele, mousePoint);
    }
}

void ROIAndVertex::scaleRect(paintElement* ele, QPoint mousePoint){
    QRectF rect = ele->rect;
    switch (this->emCurDir) {
        case HNS::DIR_LEFT:
            if(mousePoint.x()>=0)   rect.setLeft(mousePoint.x());
            break;
        case HNS::DIR_RIGHT:
            if(mousePoint.x()<=MUL_LABEL_WIDTH) rect.setRight(mousePoint.x());
            break;
        case HNS::DIR_TOP:
            if(mousePoint.y()>=0)   rect.setTop(mousePoint.y());
            break;
        case HNS::DIR_BOTTOM:
            if(mousePoint.y()<=MUL_LABEL_HEIGHT)    rect.setBottom(mousePoint.y());
            break;
        case HNS::DIR_LEFTTOP:
            if(mousePoint.x()<=0 && mousePoint.y()<=0){
                rect.setTopLeft(QPoint(0, 0));
            }else if(mousePoint.x()<=0){
                rect.setTopLeft(QPoint(0, mousePoint.y()));
            }else if(mousePoint.y()<=0){
                rect.setTopLeft(QPoint(mousePoint.x(), 0));
            }else{
                rect.setTopLeft(mousePoint);
            }
            break;
        case HNS::DIR_LEFTBOTTOM:
            if(mousePoint.x()<=0 && mousePoint.y()>=MUL_LABEL_HEIGHT){
                rect.setBottomLeft(QPoint(0, MUL_LABEL_HEIGHT));
            }else if(mousePoint.x()<=0){
                rect.setBottomLeft(QPoint(0, mousePoint.y()));
            }else if(mousePoint.y()>=MUL_LABEL_HEIGHT){
                rect.setBottomLeft(QPoint(mousePoint.x(), MUL_LABEL_HEIGHT));
            }else{
                rect.setBottomLeft(mousePoint);
            }
            break;
        case HNS::DIR_RIGHTTOP:
            if(mousePoint.x()>=MUL_LABEL_WIDTH && mousePoint.y()<=0){
                rect.setTopRight(QPoint(MUL_LABEL_WIDTH, 0));
            }else if(mousePoint.x()>=MUL_LABEL_WIDTH){
                rect.setTopRight(QPoint(MUL_LABEL_WIDTH, mousePoint.y()));
            }else if(mousePoint.y()<=0){
                rect.setTopRight(QPoint(mousePoint.x(), 0));
            }else{
                rect.setTopRight(mousePoint);
            }
            break;
        case HNS::DIR_RIGHTBOTTOM:
            if(mousePoint.x()>=MUL_LABEL_WIDTH && mousePoint.y()>=MUL_LABEL_WIDTH){
                rect.setBottomRight(QPoint(MUL_LABEL_WIDTH, MUL_LABEL_HEIGHT));
            }else if(mousePoint.x()>=MUL_LABEL_WIDTH){
                rect.setBottomRight(QPoint(MUL_LABEL_WIDTH, mousePoint.y()));
            }else if(mousePoint.y()>=MUL_LABEL_HEIGHT){
                rect.setBottomRight(QPoint(mousePoint.x(), MUL_LABEL_HEIGHT));
            }else{
                rect.setBottomRight(mousePoint);
            }
            break;
        default:
            qDebug()<<"scale default";
            break;
    }
    //限制最小的缩放
    if(rect.width()<MIN_SCALE_WIDTH || rect.height()<MIN_SCALE_HEIGHT){
        return;
    }

    ele->rect = rect;  //更新ROI矩形
    //moveStartPoint = mousePoint;    //更新鼠标起始位置
}

void ROIAndVertex::scaleLine(paintElement* ele, QPoint point){
    QLineF line = ele->line;
    //判断拖动直线的p1还是p2
    if(QLineF(line.p1(), point).length() < QLineF(line.p2(), point).length()){
        line.setP1(point);
    }else{
        line.setP2(point);
    }
    if(line.length()<3) return;
    ele->line = line;
}

void ROIAndVertex::scalePolygon(paintElement* ele, QPoint point){
    QPolygonF polygonF = ele->polygon;
    qDebug()<<"scalePolygon111";
    for(int i=0; i<polygonF.size(); i++){
        QPointF p = polygonF[i];
        QRectF rectF = QRectF(p.x()-10, p.y()-10, 20, 20);
            if(rectF.contains(point)){
                qDebug()<<"scalePolygon";
                ele->polygon[i] = point;
                return;
            }
    }
}

void ROIAndVertex::paintControlPoint(paintElement* ele, QPainter* painter){
    if(ele->paintType==RECTANGLE || ele->paintType==ELLIPSE){
        QRectF rectF = ele->rect;
        painter->drawRect(rectF.topLeft().x(), rectF.topLeft().y(), POINT_WIDTH, POINT_HEIGHT); //左上角
        painter->drawRect(rectF.topLeft().x(), rectF.topLeft().y() + rectF.height() / 2 - POINT_WIDTH / 2, POINT_WIDTH, POINT_HEIGHT); //左边中心点
        painter->drawRect(rectF.bottomLeft().x(), rectF.bottomLeft().y()- POINT_WIDTH, POINT_WIDTH, POINT_HEIGHT); //左下角
        painter->drawRect(rectF.topLeft().x() + rectF.width() / 2 - POINT_WIDTH / 2, rectF.topLeft().y(), POINT_WIDTH, POINT_HEIGHT);  //顶部中心
        painter->drawRect(rectF.topLeft().x() + rectF.width() / 2 - POINT_WIDTH /2, rectF.topLeft().y() + rectF.height() / 2 - POINT_WIDTH / 2, POINT_WIDTH, POINT_HEIGHT);  //中心点
        painter->drawRect(rectF.bottomLeft().x() + rectF.width() / 2 - POINT_WIDTH / 2, rectF.bottomLeft().y() - POINT_WIDTH, POINT_WIDTH, POINT_HEIGHT); //底部中心点
        painter->drawRect(rectF.topRight().x() - POINT_WIDTH, rectF.topRight().y(), POINT_WIDTH, POINT_HEIGHT); //右上角
        painter->drawRect(rectF.topRight().x() - POINT_WIDTH / 2, rectF.topRight().y() + rectF.height() / 2 - POINT_WIDTH /2, POINT_WIDTH, POINT_HEIGHT); //右边中心点
        painter->drawRect(rectF.bottomRight().x() - POINT_WIDTH, rectF.bottomRight().y() - POINT_WIDTH, POINT_WIDTH, POINT_HEIGHT); //右下角点
    }else if(ele->paintType==LINE){
        QLineF line = ele->line;
        QRectF rect1 = QRectF(line.p1().x()-2, line.p1().y()-2, POINT_WIDTH, POINT_HEIGHT);
        QRectF rect2 = QRectF(line.p2().x()-2, line.p2().y()-2, POINT_WIDTH, POINT_HEIGHT);
        QRectF rect3 = QRectF(line.center().x()-2, line.center().y()-2, POINT_WIDTH, POINT_HEIGHT);

        painter->fillRect(rect1, QBrush(Qt::yellow));
        painter->fillRect(rect2, QBrush(Qt::yellow));
        painter->fillRect(rect3, QBrush(Qt::yellow));
    }else if(ele->paintType==POLYGON){
        for(QPointF p:ele->polygon){
            QRectF rect = QRectF(p.x()-10, p.y()-10, 20, 20);
            painter->fillRect(rect, QBrush(Qt::yellow));
        }
    }
}

void ROIAndVertex::paintEvent(QPaintEvent *event){
    QLabel::paintEvent(event);
    QPainter* painter = new QPainter(this);
    painter->setPen(this->noSelectPen);
    bool chgPen = false;

    for(int i=0; i<paintList.size(); i++){
        paintElement* paintEle = paintList.at(i);
        if(paintEle->isActive){
            painter->save();
            painter->setPen(this->selectPen);
            chgPen = true;
        }
        QBrush brush = QBrush(HNS::baseColors[paintEle->index]);
        if(paintEle->paintType == NO_TYPE){
           // qDebug()<<"当前绘制的图形没有类型";
            continue;
        }
        if(paintEle->paintType==RECTANGLE || paintEle->paintType==ELLIPSE){
            QRect tmprect(paintEle->rect.topLeft().x()-20, paintEle->rect.topLeft().y()-20, 20, 20);
            QString str = QString("%1").arg(paintEle->index);
            painter->fillRect(tmprect, brush);
            painter->drawText(tmprect, Qt::AlignCenter, str);
            if(paintEle->paintType==RECTANGLE) painter->drawRect(paintEle->rect);
            else painter->drawEllipse(paintEle->rect);
        }else if(paintEle->paintType==LINE){
            QRect tmprect(paintEle->line.x1()-20, paintEle->line.y1()-20, 20, 20);
            QString str = QString("%1").arg(paintEle->index);
            painter->fillRect(tmprect, brush);
            painter->drawText(tmprect, str);
            painter->drawLine(paintEle->line);

        }else if(paintEle->paintType==PIPE){
            QRect tmprect(paintEle->anglePoints[0].x()-20, paintEle->anglePoints[0].y()-20, 20, 20);
            QString str = QString("P_%1").arg(paintEle->index);
            painter->drawText(tmprect, str);
            painter->drawLine(paintEle->anglePoints[0], paintEle->anglePoints[1]);
        }else if(paintEle->paintType==ANGLE){
            QPointF A = paintEle->anglePoints[0];
            QPointF B = paintEle->anglePoints[1];
            QPointF C = paintEle->anglePoints[2];
            QLineF line1(B, A);
            QLineF line2(B, C);
            qreal dotProduct = QPointF::dotProduct(line1.p2()-line1.p1(), line2.p2()-line2.p1());
            qreal len1 = line1.length();
            qreal len2 = line2.length();
            qreal angle = qAcos(dotProduct/(len1*len2));
            qreal degree = qRadiansToDegrees(angle);
           
            QRect tmprect(B.x() + 10, B.y() - 10, 100, 20);

            QString str_1 = QString::number(degree, 'f', 2);
            QString str = QString("A_%1(%2°)").arg(paintEle->index).arg(str_1);
//                if (!hasEmit[2].at(paintEle->index)) {
//                    QPair pair(paintEle->index, degree);
//                    emit signalPipeAngle(2, pair);
//                }
//                hasEmit[2][paintEle->index] = true;
                
                //str = str + str_1;
            //qDebug()<<str;
            painter->drawText(tmprect, str);
            painter->drawLine(line1);
            painter->drawLine(line2);
        }else if(paintEle->paintType==POLYGON){
            QRect tmprect(paintEle->polygon.first().x()-20, paintEle->polygon.first().y()-20, 20, 20);
            QString str = QString("%1").arg(paintEle->index);
            painter->fillRect(tmprect, brush);
            painter->drawText(tmprect, str);
            painter->drawPolygon(paintEle->polygon);
        }
        if(chgPen){
            this->paintControlPoint(paintEle, painter);

            painter->restore();
            chgPen = false;
        }
    }
    if(isPainting){
        if(drawingType==RECTANGLE)  {
            if(paintingRect.isValid())
                painter->drawRect(paintingRect);
        }
        else if(drawingType==ELLIPSE){
            if(paintingRect.isValid())
                painter->drawEllipse(paintingRect);
        }
        else if(drawingType==LINE) {
            if(paintingLine.p2().x()>0 && paintingLine.p2().y()>0)
            painter->drawLine(paintingLine);
        }
        else if(drawingType==DISTANCE){
            if(paintingLine.p2().x()>0 && paintingLine.p2().y()>0)
            painter->drawLine(paintingLine);

            QRect tmprect(paintingLine.x1()-25, paintingLine.y1()-25, 50, 20);
            QString str = QString("%1").arg(paintingLine.length());
            QBrush brush = QBrush(Qt::blue);
            painter->fillRect(tmprect, brush);
            painter->drawText(tmprect, str);
        }
        else if(drawingType==POLYGON){
            QPolygonF polygon;
            for (int i = 0; i < paintingVertex.length(); i++)   polygon.append(paintingVertex[i]);
            painter->drawPolyline(polygon);
            painter->save();
            painter->setPen(this->pointPen);
            painter->drawPoints(polygon);
            painter->restore();
        }else if(drawingType==PIPE) {
            if(paintingVertex.size()==1)    painter->drawPoint(paintingVertex[0]);
            else if(paintingVertex.size()==2){
                painter->drawPoint(paintingVertex[0]);
                painter->drawPoint(paintingVertex[1]);
                painter->drawLine(paintingVertex.at(0), paintingVertex.at(1));
            }
        }else if(drawingType==ANGLE) {
            if(paintingVertex.size()==1){
                painter->drawPoint(paintingVertex[0]);
            }else if(paintingVertex.size()==2){
                painter->drawPoint(paintingVertex[0]);
                painter->drawPoint(paintingVertex[1]);
                painter->drawLine(paintingVertex[0], paintingVertex[1]);
            }else if(paintingVertex.size()==3){
                painter->drawPoint(paintingVertex[0]);
                painter->drawPoint(paintingVertex[1]);
                painter->drawPoint(paintingVertex[2]);
                painter->drawLine(paintingVertex[0], paintingVertex[1]);
                painter->drawLine(paintingVertex[1], paintingVertex[2]);
            }
        }
        //管角是通过点击进行绘制

    }
    painter->end();
}

void ROIAndVertex::keyPressEvent(QKeyEvent* event){
    if(event->modifiers()==Qt::ControlModifier && event->key()==Qt::Key_Z){
        if(!paintList.isEmpty()){
            emit signalDelEle(paintList.last()->index);
            paintList.pop_back();
            update();
            emit signalPaintList(paintList);
        }
    }

    if(event->modifiers()==Qt::ControlModifier && event->key()==Qt::Key_C){
        int x = this->mapFromGlobal(QCursor().pos()).x();
        int y = this->mapFromGlobal(QCursor().pos()).y();
        copyPaintEle(QPoint(x, y));
    }

    if(event->modifiers()==Qt::ControlModifier && event->key()==Qt::Key_V){
        if(copyEle->paintType != NO_TYPE){
            int x = this->mapFromGlobal(QCursor().pos()).x();
            int y = this->mapFromGlobal(QCursor().pos()).y();
            pastePaintEleTopLeft = QPoint(x,y);
            pastePaintEle(pastePaintEleTopLeft);
            update();
            emit signalPaintList(paintList);
        }
    }
}

void ROIAndVertex::updateROIs(QList<paintElement*> paintList) {
    this->paintList = paintList;
    update();
}

//新建记录、打开记录，清除roi静态累加标号
void ROIAndVertex::delete_all_ROIs() //删除鼠标绘制的所有roi，新建记录、打开记录时触发
{
    nullPaintEle->clear_Index();
    this->paintList.clear(); 
    
    update();
}

void ROIAndVertex::resetStatFlag(paintElement *e, int i){
    if(e->paintType==NO_TYPE || i==-1){
        for(paintElement* ele: this->paintList){
            ele->isActive = false;
        }
        return;
    }
    for(paintElement* ele: this->paintList){
        if(ele==e){
            ele->index2 = i;
            ele->isActive = true;
            continue;
        }
        ele->isActive = false;
    }
}

