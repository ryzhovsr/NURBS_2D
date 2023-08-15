#pragma once

#include "CurvePoint.h"

class MathUtils
{
public:
    // Возвращет длину радиус-вектора
    static double calcRadiusVectorLength(double x, double y);
    static double calcRadiusVectorLength(const QPointF &point);

    // Возвращет длину вектора
    static double calcVectorLenght(double x1, double y1, double x2, double y2);
    static double calcVectorLenght(const QPointF &point1, const QPointF &point2);
    static double calcVectorLenght(const CurvePoint &point1, const QPointF &point2);

    // Возвращает угол между двумя векторами
    static double calcAngleBetweenVectors(const QPointF &vecStart1, const QPointF &vecEnd1, const QPointF &vecStart2, const QPointF &vecEnd2);

    // Рассчитывает сочетания без повторений из n по k
    static int calcCombWithoutRepetition(int n, int k);

private:
    MathUtils();
};
