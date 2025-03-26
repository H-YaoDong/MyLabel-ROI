#ifndef SHADOWLABEL_H
#define SHADOWLABEL_H

#include <QLabel>
#include <QPainter>
#include <QList>
#include <paintelement.h>

#include "AppEventMGT.h"
#include "MyNamespace.h"

class ShadowLabel : public QLabel
{
public:
    explicit ShadowLabel(QWidget *parent = nullptr);
    ShadowLabel();
    void setScale(float);    //设置缩放系数
private:
    float scale = 0.5;
    QList<paintElement*> ROIList;
    QList<paintElement> ROIListP2V(QList<paintElement *>);
    void fixPaint(QPainter*);
public slots:
    void setROIList(QList<paintElement*>);

protected:
    virtual void paintEvent(QPaintEvent *event);

};

#endif // SHADOWLABEL_H
