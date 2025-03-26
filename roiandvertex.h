#ifndef ROIANDVERTEX_H
#define ROIANDVERTEX_H

#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QMenu>
#include <QtCore/qmath.h>
#include <QDebug>

#include "paintElement.h"
#include "AppEventMGT.h"
#include "MyNamespace.h"

#define EDGPADDING 5    //四周边缘可拉伸宽度
#define CORPADDING 6    //四角可拉伸宽度

#define POINT_WIDTH      4        //边缘9点的宽度
#define POINT_HEIGHT     4        //边缘9点的高度

#define MIN_SCALE_WIDTH        30        //宽度最小可缩小到10
#define MIN_SCALE_HEIGHT       30        //长度最小可缩小到10

#define MUL_LABEL_WIDTH 612
#define MUL_LABEL_HEIGHT 512

class ROIAndVertex : public QLabel
{
    Q_OBJECT
public:
    explicit ROIAndVertex(QWidget *parent = nullptr);
    ROIAndVertex();
    void setDrawingType(PAINT_TYPE type);
    void updateROIs(QList<paintElement*>);
    void delete_all_ROIs(); //删除鼠标绘制的所有roi，新建记录时触发
private:
    QList<paintElement*> paintList;
    paintElement *nullPaintEle = new paintElement();
    paintElement *copyEle = new paintElement();    //本体

    PAINT_TYPE drawingType = NO_TYPE;   //当前正在绘制的类型
    bool isDragging = false;    //当前是否正在拖动ROI
    bool isPainting = false;    //当前是否正在绘制ROI
    bool isScaling = false;     //当前是否正在对ROI进行缩放

    bool isPaintingPolygon = false;
    paintElement* activePaintEle;

    QPoint dragStartPoint;  //拖拽的起始位置
    QPoint paintStartPoint; //绘制的起始位置
    HNS::EmDirection emCurDir = HNS::DIR_NONE;  //当前ROI缩放的方向

    QPoint copyPaintEleTopLeft; //待复制的元素的左上角位置
    QPoint pastePaintEleTopLeft;    //将元素复制的位置

    QRect paintingRect = QRect(-1,-1,-1,-1); //正在绘制的矩形，椭圆ROI
    QLineF paintingLine = QLineF(-1,-1,-1,-1);   //正在绘制的直线，管径
    QVector<QPointF> paintingVertex = QVector<QPointF>();   //正在绘制的管径和管角的点的集合

    QPen noSelectPen = QPen(QBrush(Qt::white), 2);
    QPen selectPen = QPen(QBrush(Qt::red), 2);
    QPen pointPen = QPen(QBrush(Qt::white), 6);

    QMenu* menu;    //菜单
    QAction* actDele;   //删除
    QAction* actCopy;   //复制
    QAction* actPaste;  //粘贴
    QMap<int, QVector<bool>> hasEmit; 

private:
    paintElement* check_pos(QPoint point);   //检查鼠标位置是否存在于某一绘制的区域内
    void paint(QPoint mousePoint);
    void drag(QPoint mousePoint);
    void scale(paintElement* ele, QPoint mousePoint);
    void createMenu();
    void slotMenuClicked(QAction* action);
    void copyPaintEle(QPoint);
    void pastePaintEle(QPoint);
    void resetStatFlag(paintElement *ele, int index);   //重置被选中的元素之外的全部元素的选中标志,index表示该ROI所在容器的下标

    HNS::EmDirection region(QPointF point); //看当前ROI的拖动边缘
    HNS::EmDirection rectRegion(QRectF rect, QPointF point);
    HNS::EmDirection regionLine(QLineF line, QPointF point);
    HNS::EmDirection regionPolygon(paintElement*, QPointF point);

    void scaleRect(paintElement* ele, QPoint mousePoint);
    void scaleLine(paintElement* ele, QPoint);
    void scalePolygon(paintElement* ele, QPoint);

    void paintControlPoint(paintElement* ele, QPainter* painter);
protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void keyPressEvent(QKeyEvent* event);

signals:
    void signalPaintList(QList<paintElement*> paintList);
    void signalDelEle(int count);   //发送待删除的绘画类下标
    void signalPipeAngle(int, QPair<int, qreal>);  //将绘制的管径管脚发送至主线程
    void signalDelPipeAngle(int index, paintType);  //将绘制的管径管脚发送至主线程
};

#endif // ROIANDVERTEX_H
