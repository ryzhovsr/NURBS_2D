#include "Curve.h"

Curve::Curve(const std::vector<QPointF> &controlPoints, const std::vector<double> &weights, int degree, int curveNumPoints) noexcept
    : _controlPoints {controlPoints}, _weights { weights }, _degree { degree }, _numRealRangePoints { curveNumPoints }
{
    _pointsNURBS.resize(_numRealRangePoints); // Устанавливаем количество точек, из которых будет состоять кривая

    _numVertices = static_cast<int>(_controlPoints.size()); // Кол-во вершин определяющего многоугольника
    _numKnots = _numVertices + _degree + 1; // Общее кол-во узлов узлового вектора
    _numRealRangeKnots = _numVertices - _degree + 1; // Кол-во узлов реального диапазона узлового вектора

    // Начало и конец реального диапазона узлового вектора
    _realRangeStart = _degree;
    _realRangeEnd = _numKnots - _degree - 1;
}

void Curve::calcCurve()
{

}
