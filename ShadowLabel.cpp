#include "ShadowLabel.h"

ShadowLabel::ShadowLabel(QWidget *parent)
    : QLabel{parent}
{
    connect(AppEventMGT::getInstance(), &AppEventMGT::s_body2shadow, this, &ShadowLabel::setROIList);
}

ShadowLabel::ShadowLabel()
{
}

void ShadowLabel::setScale(float scale){
    this->scale = scale;
}

void ShadowLabel::setROIList(QList<paintElement*> ROIList){
    this->ROIList = ROIList;
    update();
}

void ShadowLabel::paintEvent(QPaintEvent *event){
    QLabel::paintEvent(event);
    QPainter *painter = new QPainter(this);

    QBrush brush = QBrush(Qt::white);
    QPen pen = QPen(brush, 2);
    painter->setPen(pen);
    this->fixPaint(painter);
    painter->end();
}

void ShadowLabel::fixPaint(QPainter* painter){
    float scale = 0.5;
    for(int i=0; i<this->ROIList.size(); i++){
        paintElement* ele = this->ROIList[i];
        if(ele->paintType == NO_TYPE){
            continue;
        }
        QBrush tipRectBrush = QBrush(HNS::baseColors[ele->index]);
        if(ele->paintType==RECTANGLE || ele->paintType==ELLIPSE){
            QPointF topLeft = ele->rect.topLeft()*scale;
            QPointF bottomRight = ele->rect.bottomRight()*scale;
            QRect tipRect(topLeft.x()-20, topLeft.y()-20, 20, 20);

            painter->fillRect(tipRect, tipRectBrush);
            QString str = QString("%1").arg(ele->index);
            painter->drawText(tipRect, Qt::AlignCenter, str);
            QRectF tmpRect = QRectF(topLeft, bottomRight);
            if(ele->paintType==RECTANGLE) {
                painter->drawRect(tmpRect);
            }else{
                painter->drawEllipse(tmpRect);
            }
        }else if(ele->paintType==LINE){
            QRectF tipRect(ele->line.x1()*this->scale-20, ele->line.y1()*this->scale-20, 20, 20);
            QString str = QString("%1").arg(ele->index);
            painter->fillRect(tipRect, tipRectBrush);
            painter->drawText(tipRect, Qt::AlignCenter, str);
            QLineF tmpLine = QLineF(ele->line.p1()*scale, ele->line.p2()*scale);
            painter->drawLine(tmpLine);
        }else if(ele->paintType==PIPE){
            QRect tmprect(ele->anglePoints[0].x()-20, ele->anglePoints[0].y()-20, 40, 20);
            QString str = QString("P_%1").arg(ele->index);
//            painter->drawText(tmprect, str);

            painter->drawLine(ele->anglePoints[0]*scale, ele->anglePoints[1]*scale);
        }else if(ele->paintType==ANGLE){
            QPointF A = ele->anglePoints[0]*scale;
            QPointF B = ele->anglePoints[1]*scale;
            QPointF C = ele->anglePoints[2]*scale;
            QLineF line1(B, A);
            QLineF line2(B, C);
            qreal dotProduct = QPointF::dotProduct(line1.p2()-line1.p1(), line2.p2()-line2.p1());
            qreal len1 = line1.length();
            qreal len2 = line2.length();
            qreal angle = qAcos(dotProduct/(len1*len2));
            qreal degree = qRadiansToDegrees(angle);

            QRect tmprect(B.x() + 10, B.y() - 10, 100, 20);
            QString str_1 = QString::number(degree, 'f', 2);
//            QString str = QString("A_%1(%2°)").arg(ele->index).arg(str_1);
//            painter->drawText(tmprect, str);
            painter->drawLine(line1);
            painter->drawLine(line2);
        }else if(ele->paintType==POLYGON){
            QRect tipRect((ele->polygon.first()*this->scale).x()-20, (ele->polygon.first()*this->scale).y()-20, 20, 20);
            QString str = QString("%1").arg(ele->index);
            painter->drawText(tipRect, str);
            painter->fillRect(tipRect, tipRectBrush);
            QPolygonF tmpPolygonF;
            for(QPointF p: ele->polygon) tmpPolygonF.append(p*scale);
            painter->drawPolygon(tmpPolygonF);
        }
    }

}
