#pragma once

#include "CurvePoint.h"
#include <vector>

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
    void _calcCurvePointAndDerivs(CurvePoint &curvePoint, double realPoint);     // Вычисляет точку кривой и её производные
    int _findSpanForRealPoint(double realPoint);    // Находит номер узлового промежутка (спан) для точки реального диапазона узл. вектора
    std::vector<std::vector<double>> _calcBasisFuncsAndTheirDerivs(double realPoint, double span);    // Вычисляет базисные функции и их производные
    void _calcBasisFuncs(std::vector<std::vector<double>> &basisFuncsAndTheirDerivs, std::vector<std::vector<double>> &tempStorage, double realPoint, double span); // Вычисляет базисные функции
    void _calcDerivsBasisFuncs(std::vector<std::vector<double>> &basisFuncsAndTheirDerivs, std::vector<std::vector<double>> &tempStorage);  // Вычисляет производные базисных функций
    void calcPointCurve(const std::vector<std::vector<double>> &basisFunctionsAndTheirDerivs, CurvePoint &curvePoint, double &denominator, QPointF &n0); // Вычисляет точку кривой
    // Вычисляют первую и вторую производные в точке кривой
    void _calcPointFirstDeriv(const std::vector<std::vector<double>> &basisFuncsAndTheirDerivs, CurvePoint &curvePoint, double denominator, QPointF &n0, QPointF &n1, double n2);
    void _calcPointSecondDeriv(const std::vector<std::vector<double>> &basisFuncsAndTheirDerivs, CurvePoint &curvePoint, double denominator, QPointF &n0, QPointF &n1, double n2);
};
