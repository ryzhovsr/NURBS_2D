#include "MathUtils.h"
#include <QtMath>

double MathUtils::calcRadiusVectorLength(double x, double y)
{
    return sqrt(x * x + y * y);
}

double MathUtils::calcRadiusVectorLength(const QPointF &point)
{
    return calcRadiusVectorLength(point.x(), point.y());
}

double MathUtils::calcVectorLenght(double x1, double y1, double x2, double y2)
{
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

double MathUtils::calcVectorLenght(const QPointF &point1, const QPointF &point2)
{
    return calcVectorLenght(point1.x(), point1.y(), point2.x(), point2.y());
}

double MathUtils::calcVectorLenght(const CurvePoint &point1, const QPointF &point2)
{
    return calcVectorLenght(point1.point.x(), point1.point.y(), point2.x(), point2.y());
}

double MathUtils::calcAngleBetweenVectors(const QPointF &vecStart1, const QPointF &vecEnd1, const QPointF &vecStart2, const QPointF &vecEnd2)
{
    double x1 = vecEnd1.x() - vecStart1.x();
    double y1 = vecEnd1.y() - vecStart1.y();
    double x2 = vecEnd2.x() - vecStart2.x();
    double y2 = vecEnd2.y() - vecStart2.y();
    double numerator = x1 * x2 + y1 * y2;
    double vecLen1 = calcVectorLenght(vecStart1, vecEnd1);
    double vecLen2 = calcVectorLenght(vecStart2, vecEnd2);
    double cos = numerator / (vecLen1 * vecLen2);
    return acos(cos) * 180 / M_PI;
}

int MathUtils::calcCombWithoutRepetition(int n, int k)
{
    return k == 0 || k == n ? 1 : calcCombWithoutRepetition(n - 1, k - 1) * n / k;
}
