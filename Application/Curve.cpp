#include "Curve.h"
#include <QDebug>
#include "CalcCurve.h"

Curve::Curve(const std::vector<QPointF> &controlPoints, const std::vector<double> &weights, int degree, int curveNumPoints) noexcept
    : _controlPoints {controlPoints}, _weights { weights }, _degree { degree }, _numRealRangePoints { curveNumPoints }
{
    _curvePoints.resize(_numRealRangePoints); // Устанавливаем количество точек, из которых будет состоять кривая

    _numVertices = static_cast<int>(_controlPoints.size()); // Кол-во вершин определяющего многоугольника
    _numRealRangeKnots = _numVertices - _degree + 1;        // Кол-во узлов реального диапазона узл. вектора
    _numKnots = _numVertices + _degree + 1;                 // Общее кол-во узлов узл. вектора

    // Начало и конец реального диапазона узл. вектора
    _realRangeStart = _degree;
    _realRangeEnd = _numKnots - _degree - 1;

    double step = 1.0 / (_numRealRangeKnots - 1); // Шаг в реальном диапазоне узл. вектора
    _nodalVector.resize(_numKnots);

    for (int i = _realRangeStart + 1; i < _realRangeEnd; ++i)    // Заполняем реальный диапазон узл. вектора
        _nodalVector[i] = _nodalVector[i - 1] + step;

    for (int i = _realRangeEnd; i < _numKnots; ++i)    // Заполняем оставшуюся часть узл. вектора единицами
        _nodalVector[i] = 1;

    _checkNodalVector();
    calcCurve();
}

void Curve::setNodalVector(const std::vector<double> &nodalVector)
{
    _nodalVector = nodalVector;
    _checkNodalVector();
}

std::vector<CurvePoint> Curve::getCurvePoints() const noexcept
{
    return _curvePoints;
}

std::vector<QPointF> Curve::getControlPoints() const noexcept
{
    return _controlPoints;
}

std::vector<double> Curve::getNodalVector() const noexcept
{
    return _nodalVector;
}

std::vector<double> Curve::getWeights() const noexcept
{
    return _weights;
}

int Curve::getDegree() const noexcept
{
    return _degree;
}

void Curve::_checkNodalVector()
{
    for (int i = 0; i < _numKnots - 1; ++i) // Проверка узлового вектора на убывание
    {
        if (_nodalVector[i] > _nodalVector[i + 1])
            qDebug() << "Error! _checkNodalVector: узловой вектор убывает _nodalVector[i] > _nodalVector[i + 1]!";
    }

    if (_numKnots != (_numVertices + _degree + 1))
        qDebug() << "Error! _checkNodalVector: nodalVector.size()) != (numVertices + degreeCurve + 1)!";
}

void Curve::calcCurve()
{
    for (int i = 0; i < _numRealRangePoints; ++i) // Итерируемся по каждой точке кривой
    {
        // Находим точку реальной части узл. вектора (параметр кривой ∈ [0, 1])
        double parameter = static_cast<double>(i) / (_numRealRangePoints - 1) * (_nodalVector[_realRangeEnd] - _nodalVector[_realRangeStart]) + _nodalVector[_realRangeStart];
        CalcCurve::calcCurvePointAndDerivs(*this, _curvePoints[i], parameter); // Рассчитываем точку кривой и её первую и вторую производную
    }
}
