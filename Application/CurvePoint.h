#pragma once

#include <QPointF>

struct CurvePoint
{
    QPointF point;        // Координаты точки кривой
    QPointF firstDeriv;   // Координаты 1-ой производной
    QPointF secondDeriv;  // Координаты 2-ой производной
    double parameter;     // Параметр реальной части узл. вектора
    int span;   // Номер интервала, к которому относится parameter
};
