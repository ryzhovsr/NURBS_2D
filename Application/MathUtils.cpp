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

std::pair<std::vector<CurvePoint>, std::vector<CurvePoint>> MathUtils::moveCurve(const Curve &curve, double length)
{
    int numRealRangePoints = static_cast<int>(curve.getCurvePoints().size());
    std::vector<CurvePoint> movePointsNURBS(numRealRangePoints); // Точки NURBS со сдвигом длиной length
    std::vector<CurvePoint> reverseMovePointsNURBS(numRealRangePoints); // Точки NURBS со сдвигом в противоположную сторону длиной length

    for (size_t i = 0; i < curve.getCurvePoints().size(); ++i)
    {
        CurvePoint curvePoint = curve.getCurvePoints()[i];
        double rotatedX = curvePoint.firstDeriv.x() * cos(M_PI / 2) - curvePoint.firstDeriv.y() * sin(M_PI / 2);
        double rotatedY = curvePoint.firstDeriv.x() * sin(M_PI / 2) + curvePoint.firstDeriv.y() * cos(M_PI / 2);
        QPointF rotatedPoint =  QPointF(rotatedX + curvePoint.point.x(), rotatedY + curvePoint.point.y());  // Точка кривой со повотором на 90 градусов от начала коор-т

        double x = rotatedPoint.x() - curvePoint.point.x();
        double y = rotatedPoint.y() - curvePoint.point.y();
        double vecLen = MathUtils::calcRadiusVectorLength(x, y);
        x *= length / vecLen;
        y *= length / vecLen;

        movePointsNURBS[i].point = QPointF(x + curvePoint.point.x(), y + curvePoint.point.y());

        rotatedX = curvePoint.firstDeriv.x() * cos(-M_PI / 2) - curvePoint.firstDeriv.y() * sin(-M_PI / 2); // Точка кривой со сдвигом на -90 градусов от начала коор-т
        rotatedY = curvePoint.firstDeriv.x() * sin(-M_PI / 2) + curvePoint.firstDeriv.y() * cos(-M_PI / 2);

        QPointF reverseRotatedPoints = QPointF(rotatedX + curvePoint.point.x(), rotatedY + curvePoint.point.y());

        x = reverseRotatedPoints.x() - curvePoint.point.x();
        y = reverseRotatedPoints.y() - curvePoint.point.y();
        vecLen = MathUtils::calcRadiusVectorLength(x, y);
        x *= length / vecLen;
        y *= length / vecLen;

        reverseMovePointsNURBS[i].point = QPointF(x + curvePoint.point.x(), y + curvePoint.point.y());
    }

    return std::pair<std::vector<CurvePoint>, std::vector<CurvePoint>> (movePointsNURBS, reverseMovePointsNURBS);
}
