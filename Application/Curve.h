#pragma once

#include <vector>
#include <QPointF>

struct CurvePoint
{
    QPointF point;        // Координаты точки кривой
    QPointF firstDeriv;   // Координаты 1-ой производной
    QPointF secondDeriv;  // Координаты 2-ой производной
    double parameter;     // Элемент реальной части узлового вектора
    int span;   // Номер интервала, к которому относится CurvePoint
};

class Curve
{
public:
    Curve(const std::vector<QPointF> &controlPoints, const std::vector<double> &weights, int degree, int curveNumPoints) noexcept;

    void calcCurve(); // Рассчитывает точки кривой

private:
    std::vector<CurvePoint> _curvePoints;   // Точки кривой
    std::vector<QPointF> _controlPoints;    // Точки определяющего многоугольника
    std::vector<double> _nodalVector;       // Узловой вектор
    std::vector<double> _weights;           // Весовые коэффициенты точек определяющего многоугольника
    int _realRangeStart, _realRangeEnd;     // Начало/конец реального диапазона узл. вектора
    int _numRealRangeKnots;     // Кол-во узлов (длина) реальной части узлового вектора
    int _numRealRangePoints;    // Кол-во точек (разбиений) в реальной части узлового вектора
    int _numVertices;   // Кол-во вершин определяющего многоугольника
    int _numKnots;      // Кол-во узлов в узл. векторе
    int _degree;        // Степень кривой

    void _checkNodalVector(); // Проверяет узловой вектор на соответствие

    // Методы для расчёта точек кривой
    void _calcCurvePointAndItsDerivs(CurvePoint &curvePoint, double realPoint);     // Заменить название!!!!!!!!!!!
    int _findSpanForRealPoint(double realPoint);    // Находит номер злового промежутка (спан) для точки реального диапазона узл. вектора
    void _calcBasisFuncsAndTheirDerivs(std::vector<std::vector<double>> &basisFuncsAndTheirDerivs, double realPoint, double span);    // Вычисляет базисные функции и их производные
    void _calcBasisFuncs(std::vector<std::vector<double>> &basisFuncsAndTheirDerivs, std::vector<std::vector<double>> &tempStorage, double realPoint, double span);
    void _calcDerivsBasisFuncs(std::vector<std::vector<double>> &basisFuncsAndTheirDerivs, std::vector<std::vector<double>> &tempStorage);

    void _calcCurvePoint(const std::vector<std::vector<double>> &basisFuncsAndTheirDerivs, CurvePoint &curvePoint, double& denominator, QPointF &n0);
    void calcFirstDerivCurve(const std::vector<std::vector<double>> &basisFuncsAndTheirDerivs, CurvePoint &curvePoint, double denominator, QPointF &n0, QPointF &n1, double n2);
    void calcSecondDerivCurve(const std::vector<std::vector<double>> &basisFuncsAndTheirDerivs, CurvePoint &curvePoint, double denominator, QPointF &n0, QPointF &n1, double n2);


};
