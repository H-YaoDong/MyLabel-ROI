#ifndef APPEVENTMGT_H
#define APPEVENTMGT_H

#include <QObject>
#include <QDebug>
#include "paintelement.h"

class AppEventMGT: public QObject
{
    Q_OBJECT

public:
    static AppEventMGT* getInstance();
    static void deleInstance();

private:
    AppEventMGT();
    ~AppEventMGT();

    // 将其拷贝构造和赋值构造成为私有函数, 禁止外部拷贝和赋值
    AppEventMGT(const AppEventMGT &signal);
    const AppEventMGT &operator=(const AppEventMGT &signal);

private:
    static AppEventMGT *p_singleton;

public slots:
    void trfROIList(QList<paintElement*>);

signals:
    void s_body2shadow(QList<paintElement*>);
};

#endif // APPEVENTMGT_H
