#pragma once

#include <QPointF>
#include <vector>

struct CurvePoint
{
    std::vector<QPointF> derivs;
    double x, y;
    double parameter;     // Параметр реальной части узл. вектора
    int span;   // Номер интервала, к которому относится parameter
};
