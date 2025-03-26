#ifndef PAINTELEMENT_H
#define PAINTELEMENT_H

#include <QRect>
#include <QVector>
#include <QLine>
#include <QPolygonF>
#include <QDebug>

typedef enum paintType{
    RECTANGLE = 1,    //画矩形
    ELLIPSE, //画椭圆
    LINE,       //画线
    ANGLE,  //角度
    PIPE,   //管径
    POLYGON,    //多边形
    DISTANCE,   //测距
    NO_TYPE
}PAINT_TYPE;

class paintElement
{
public:
    paintElement();
    paintElement(QRect rect, PAINT_TYPE paintType=NO_TYPE);
    paintElement(QLineF line, PAINT_TYPE paintType=NO_TYPE);
    paintElement(QPolygonF polygon, PAINT_TYPE paintType=NO_TYPE);

    paintElement(QVector<QPointF> anglePoints, PAINT_TYPE paintType=NO_TYPE);
    paintElement(QRect rect, QLineF line, QVector<QPoint> anglePoints, PAINT_TYPE paintType);
    void clear_Index();
    void setIndex(int index);

//    bool containPoint(QPointF);    //检测点击的点是否存在于ROI内
public:
    static int roiIndex;   //用来给椭圆，矩形，直线进行编号，每创建一次该类型后自动+1
    static int lineIndex;   //用于给直线进行编号
    static int angleIndex;  //为管角进行编号
    static int pipeIndex;   //为管径进行编号
    static int polygonIndex;    //为多边形进行编号

    QRectF rect;     //表示椭圆，矩形位置的矩形
    QLineF line;     //表示管径，直线的位置
    QPolygonF polygon;  //表示多边形所在的位置
    PAINT_TYPE paintType = NO_TYPE;
    QVector<QPointF> anglePoints;    //表示直线，角度，管径的容器

    int index = 0;  //roi，管径，管角在对应类别中的下标
    int index2 = -1;  //当前创建的对象在paintList中的下标
    int angle = -1; //表示当前ROI是在旋转了多少角度的情况下被创建的
    bool isActive = false;  //表示当前是否被选中
};

#endif // PAINTELEMENT_H
