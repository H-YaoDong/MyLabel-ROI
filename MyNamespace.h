#pragma once

#ifndef MYNAMESPACE_H
#define MYNAMESPACE_H

#include <QtCore/qglobal.h>
#include <qcolor.h>

QT_BEGIN_NAMESPACE
namespace HNS{
    extern QList<QColor> baseColors;    //曲线的颜色
    //ROI的拖动方向
    enum EmDirection
    {
        DIR_TOP = 0,
        DIR_BOTTOM,
        DIR_LEFT,
        DIR_RIGHT,
        DIR_LEFTTOP,
        DIR_LEFTBOTTOM,
        DIR_RIGHTTOP,
        DIR_RIGHTBOTTOM,
        DIR_MIDDLE, //鼠标位于矩形ROI区域中
        DIR_FREE,   //对于直线和多边形ROI的缩放
        DIR_NONE
    };
}


QT_END_NAMESPACE

#endif
