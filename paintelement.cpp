#include "paintelement.h"

//类的声明不会进行内存空间的分配，类的静态成员无法在类声明中定义。因此，类的静态成员需要在类内声明，类外定义。
int paintElement::roiIndex = 0;
int paintElement::lineIndex = 0;
int paintElement::angleIndex = 0;
int paintElement::pipeIndex = 0;
int paintElement::polygonIndex = 0;

paintElement::paintElement(){

}

paintElement::paintElement(QRect rect, PAINT_TYPE paintType){
    this->rect = rect;
    this->paintType = paintType;
    this->index = ++roiIndex;
}

paintElement::paintElement(QLineF line, PAINT_TYPE paintType){
    this->line = line;
    this->paintType = paintType;
//    index = ++lineIndex;    //lineROI的编号直接增加1000，用于与ROI进行区分，这样以来，不需要paintType，即可判断出绘制的ROI类型
    this->index = ++roiIndex;
}

paintElement::paintElement(QPolygonF polygon, PAINT_TYPE paintType){
    this->polygon = polygon;
    this->paintType = paintType;
    this->index = ++roiIndex;
}

paintElement::paintElement(QVector<QPointF> anglePoints, PAINT_TYPE paintType){
    this->anglePoints = anglePoints;
    this->paintType = paintType;
    if(paintType==ANGLE)    this->index = ++angleIndex;
    else if(paintType==PIPE) this->index = ++pipeIndex;
}

void paintElement::setIndex(int index) {
    this->index = index;
}

//新建记录、打开记录，清除roi静态累加标号
void paintElement::clear_Index() 
{
    roiIndex = 0;
    angleIndex = 0;
    pipeIndex = 0;
    lineIndex = 0;
    polygonIndex = 0;
}
