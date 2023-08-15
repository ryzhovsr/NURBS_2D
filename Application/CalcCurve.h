#pragma once

#include <vector>
#include <QPointF>
#include "Curve.h"

class CalcCurve
{
public:
    static void calcCurvePointAndDerivs(const Curve &curve, CurvePoint &curvePoint, double realPoint);     // Вычисляет точку кривой и её производные

    static int findSpanForParameter(double parameter, const std::vector<double> &nodalVector, int degree);    // Находит номер узлового промежутка (спан) для точки реального диапазона узл. вектора - параметра

    static std::vector<std::vector<double>> calcBasisFuncsAndTheirDerivs(const std::vector<double>& nodalVector, double parameter, int span, int degree);    // Вычисляет базисные функции и их производные

    static void calcBasisFuncs(std::vector<std::vector<double>> &basisFuncsAndTheirDerivs, std::vector<std::vector<double>> &tempStorage, const std::vector<double>& nodalVector, double parameter, int span, int degree); // Вычисляет базисные функции

    static void calcDerivsBasisFuncs(std::vector<std::vector<double>> &basisFuncsAndTheirDerivs, std::vector<std::vector<double>> &tempStorage, int degree);  // Вычисляет производные базисных функций

    static void calcPointCurve(const Curve &curve, const std::vector<std::vector<double>> &basisFunctionsAndTheirDerivs, CurvePoint &curvePoint, double &denominator, QPointF &n0); // Вычисляет точку кривой

    // Вычисляют первую и вторую производные в точке кривой
    static void calcPointFirstDeriv(const Curve &curve, const std::vector<std::vector<double>> &basisFuncsAndTheirDerivs, CurvePoint &curvePoint, double denominator, QPointF &n0, QPointF &n1, double n2);
    static void calcPointSecondDeriv(const Curve &curve, const std::vector<std::vector<double>> &basisFuncsAndTheirDerivs, CurvePoint &curvePoint, double denominator, QPointF &n0, QPointF &n1, double n2);
};
