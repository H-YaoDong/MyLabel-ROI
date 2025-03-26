#include "AppEventMGT.h"

AppEventMGT* AppEventMGT::p_singleton = new (std::nothrow) AppEventMGT();

AppEventMGT::AppEventMGT()
{
}

AppEventMGT::~AppEventMGT()
{
    this->deleInstance();
}

AppEventMGT* AppEventMGT::getInstance()
{
    return p_singleton;
}

void AppEventMGT::deleInstance()
{
    if (p_singleton)
    {
        free(p_singleton);
        //delete p_singleton;
        p_singleton = nullptr;
    }
}

void AppEventMGT::trfROIList(QList<paintElement*> ROIList){
//    qDebug()<<"AppEventMGT::trfROIList";
    emit s_body2shadow(ROIList);
}


