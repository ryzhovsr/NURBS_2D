#pragma once

#include <vector>
#include <QPointF>
#include "Curve.h"

class CalcCurve
{
public:
    // Вычисляет точку кривой и её производные
    static void calcCurvePointAndDerivs(const Curve &curve, CurvePoint &curvePoint, double parameter);

    // Находит номер узлового промежутка (спан) для точки реального диапазона узл. вектора - параметра
    static int findSpanForParameter(double parameter, const std::vector<double> &nodalVector, int degree);

    // Вычисляет базисные функции и их производные
    static std::vector<std::vector<double>> calcBasisFuncsAndTheirDerivs(const std::vector<double>& nodalVector, double parameter, int span, int degree);

    // Вычисляет базисные функции
    static void calcBasisFuncs(std::vector<std::vector<double>> &basisFuncsAndTheirDerivs, std::vector<std::vector<double>> &tempStorage, const std::vector<double>& nodalVector, double parameter, int span, int degree);

    // Вычисляет производные базисных функций
    static void calcDerivsBasisFuncs(std::vector<std::vector<double>> &basisFuncsAndTheirDerivs, std::vector<std::vector<double>> &tempStorage, int degree);

    // Вычисляет точку кривой
    static void calcPointCurve(const Curve &curve, const std::vector<std::vector<double>> &basisFunctionsAndTheirDerivs, CurvePoint &curvePoint, double &denominator, QPointF &n0);

    // Вычисляют первую и вторую производные в точке кривой
    static void calcPointFirstDeriv(const Curve &curve, const std::vector<std::vector<double>> &basisFuncsAndTheirDerivs, CurvePoint &curvePoint, double denominator, QPointF &n0, QPointF &n1, double n2);
    static void calcPointSecondDeriv(const Curve &curve, const std::vector<std::vector<double>> &basisFuncsAndTheirDerivs, CurvePoint &curvePoint, double denominator, QPointF &n0, QPointF &n1, double n2);
};
