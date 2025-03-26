## 介绍

绘制矩形，椭圆，直线实际用的都是是同一个思路：鼠标第一次点击就确定了本次绘制的矩形（椭圆，直线）的位置，然后拖动鼠标生成矩形（椭圆，直线），最后释放鼠标，矩形（椭圆，直线）就绘制完成了。而多边形不同，绘制多边形是一种离散的操作，多边形是由多个离散的点组成，靠的是鼠标点击绘制，而不是拖动绘制，因此文章只介绍矩形的多边形的绘制方法。

![](images\1.gif)

## 绘制一个矩形（椭圆）roi

继承QLabel，重写  paintEvent，mouseMoveEvent，mousePressEvent方法。

绘制矩形（椭圆）roi的思路是：设置鼠标点击的位置为矩形roi的左上角（mousePressEvent）--->鼠标在移动的过程中更新矩形roi的右下角（mouseMoveEvent）--->更新QLabel上的内容（paintEvent）

```c++
void MyLabel::paintEvent(QPaintEvent *event){
    QLabel::paintEvent(event);
    QPainter* painter = new QPainter(this);
    painter->drawRect(rect);
    //绘制以rect为外接矩形的椭圆
    //painter->drawEllipse(rect);
    painter->end();	//调用QLabel的update方法，刷新内容
}
void MyLabel::mouseMoveEvent(QMouseEvent *event){
    rect.setBottomRight(event->pos());
    update();
}	
void MyLabel::mousePressEvent(QMouseEvent *event){
    rect.setTopLeft(event->position());
}
```

这样，就能绘制一个矩形（椭圆）roi了。绘制直线的过程也符合上述逻辑：首先，设置鼠标点击的位置为线的第一个点；然后，在鼠标移动的过程中，不断地更新线的第二个点；最后，再调用update方法更新即可。

![](images\4.gif)

## 绘制一个多边形roi

绘制多边形roi的思路与上述的矩形，椭圆，直线就不一样。由于多边形是离散的多个点，我们就需要使用一个容器来存储这些点。实际上，在QT实现的QPolygon类中，也是用一个QList链表来存储这些节点。

![](images\1.png)

绘制多边形roi的思路是：用链表记录鼠标每次点击的位置（mousePressEvent）---->绘制多边形的中的点和线段（paintEvent），注意在绘制完之前，多边形roi不是闭合的。

```c++
void MyLabel::paintEvent(QPaintEvent *event){
    QLabel::paintEvent(event);
    QPainter* painter = new QPainter(this);
    QPolygonF polygon;
    for (int i = 0; i < myPolygon.length(); i++)   polygon.append(myPolygon[i]);
    painter->drawPolyline(polygon);
    painter->drawPoints(polygon);
    painter->end();
}
void MyLabel::mousePressEvent(QMouseEvent *event){
    if(event->button()==Qt::LeftButton)
        myPolygon.append(event->pos());
    else if(event->button()==Qt::RightButton){
        //多边形绘制完毕
    }
}
```

![](images\2.gif)

## 对矩形roi的缩放：

当鼠标在以下两个范围时，就可以对roi的大小进行缩放

1. 距离矩形roi顶点周围CORPADDING像素大小
2. 在矩形的边内外EDGPADDING像素大小

![](images\5.png)

由于拖动不同的边和不同的顶点，矩形的缩放行为也不一样（往左拖动左边，矩形就应该向左放大；往右拖动左边，矩形就应该向右缩小）。为了清楚当前是拖动矩形roi的那条边，那个顶点，就需要引入方向枚举变量。

```c++
enum EmDirection{
	DIR_TOP = 0,	//鼠标位于矩形的左边
	DIR_BOTTOM,		//鼠标位于矩形的右边
    DIR_LEFT,	
	DIR_RIGHT,
    DIR_LEFTTOP,	//鼠标位于矩形的左上方顶点
	DIR_LEFTBOTTOM,	//鼠标位于矩形的左下方顶点
    DIR_RIGHTTOP,
	DIR_RIGHTBOTTOM,
    DIR_MIDDLE, //鼠标位于矩形ROI区域中
    DIR_NONE	//鼠标距离roi较远的位置
};
```

判断鼠标当前拖动的方向：

```c++
EmDirection rectRegion(QRectF rect, QPointF point){
    int mouseX = point.x();
    int mouseY = point.y();
    QPointF roiTopLeft = rect.topLeft();
    QPointF roiBottomRight = rect.bottomRight();

    EmDirection dir = DIR_NONE;
    if (mouseX <= roiTopLeft.x() + CORPADDING && mouseX >= roiTopLeft.x() && mouseY <= roiTopLeft.y() + CORPADDING && mouseY >= roiTopLeft.y())
    {   //左上角
        this->setCursor(Qt::SizeFDiagCursor);
        dir = DIR_LEFTTOP;
    }
	//接着再判断  右上角，左下角，右下角 的情况
    else if (mouseX >= roiBottomRight.x() - EDGPADDING && mouseX <= roiBottomRight.x() && mouseY >= roiTopLeft.y() && mouseY <= roiBottomRight.y())
    {   //右边
        this->setCursor(Qt::SizeHorCursor);
        dir = DIR_RIGHT;
    }
    //再判断 上边，下边，左边 的情况
    else if (mouseY <= roiTopLeft.y() + EDGPADDING && mouseY >= roiTopLeft.y() && mouseX >= roiTopLeft.x() && mouseX <= roiBottomRight.x())
    else if(rect.contains(point))    
    {	//内部
        dir = DIR_MIDDLE;
    }else{
        dir = DIR_NONE;
    }
    return dir;
}
```

在获取了缩放的方向之后，就可以进行缩放操作了：

```c++
void scaleRect(QRectF rect, QPoint mousePoint){
    switch (this->emCurDir) {
        case DIR_LEFT:	//向左缩放
            if(mousePoint.x()>=0)   rect.setLeft(mousePoint.x());
            break;
        case DIR_RIGHT:	//向右缩放
            if(mousePoint.x()<=MUL_LABEL_WIDTH) rect.setRight(mousePoint.x());
            break;
        case DIR_TOP:	//向上缩放
            if(mousePoint.y()>=0)   rect.setTop(mousePoint.y());
            break;
        case DIR_BOTTOM:	//向下缩放
            if(mousePoint.y()<=MUL_LABEL_HEIGHT)    rect.setBottom(mousePoint.y());
            break;
        case DIR_LEFTTOP:	//拖动矩形的左上顶点
            if(mousePoint.x()<=0 && mousePoint.y()<=0){	//这些代码是做了边界处理，限制了roi只能在QLabel内
                rect.setTopLeft(QPoint(0, 0));
            }else if(mousePoint.x()<=0){
                rect.setTopLeft(QPoint(0, mousePoint.y()));
            }else if(mousePoint.y()<=0){
                rect.setTopLeft(QPoint(mousePoint.x(), 0));
            }else{
                rect.setTopLeft(mousePoint);
            }
            break;
        case DIR_LEFTBOTTOM:	//拖动矩形的右下顶点
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
        case DIR_RIGHTTOP:	//拖动矩形的右上顶点
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
        case DIR_RIGHTBOTTOM:	//拖动矩形的右下顶点
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
    this->rect = rect;  //更新ROI矩形
}

```

对椭圆roi的缩放实际上就是对其外接矩形缩放，原理和矩形roi一样，这里就不再介绍。

## 对多边形rio的缩放（移动点的位置）

如果鼠标在多边形点周围CORPADDING像素范围的大小，那么鼠标就可以拖动该顶点，以修改多边形的形状和大小

![](images\3.png)

当找到了拖动的那个点后，再进行拖动，就能缩放多边形roi了。

```c++
void ROIAndVertex::scalePolygon(QPolygonF polygonF, QPoint point){
    for(int i=0; i<polygonF.size(); i++){
        QPointF p = polygonF[i];
        QRectF rectF = QRectF(p.x()-CORPADDING, p.y()-CORPADDING, 2*CORPADDING, 2*CORPADDING);
            if(rectF.contains(point)){
                ele->polygon[i] = point;
                return;
            }
    }
}
```

