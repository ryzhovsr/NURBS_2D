#pragma once

#include "IApproxAlgs.h"

class AlgBasedCurveConjugation : public IApproxAlgs
{
public:
    // Аппроксимирует кривую: добавляет в исхождную кривую кратный узел, делит на 2 Безье, понижает их степень, соединяет, и по новой
    Curve approximateCurve(const Curve &curve, int degreeApprox) const override;

private:
    void addNodalPoints(Curve &curve) const; // Добавляет в узловой вектор новые узлы

    // Переопределяет кривую по новым контрольным точкам, узловому вектору и степени
    Curve redefineCurve(const std::vector<QPointF> &controlPoints, const std::vector<double> &nodalVector, int degree, int numParameters) const;

    // Понижает степень кривой Безье на 1
    void reductBezierCurveDegree(Curve &curve) const;

    // Соединяет кривые обычным методом меняя все исходные контрольные точки
    Curve attachCurve(const Curve &curve1, const Curve &curve2, bool fixateStartEndPoints) const;

    // Рекурсивные методы рассчёта производных, предназначенных для дальнейшего соединения кривых Безье
    QPointF calcDerivLeftBezierCurveForMerger(const std::vector<QPointF>& points, int currentIndex, int startIndex) const;
    QPointF calcNegativeDerivLeftBezierCurveForMerger(const std::vector<QPointF>& points, int currentIndex, int startIndex) const; // Возвращает отрицательную производную для левой кривой
    QPointF calcDerivRightBezierCurveForMerger(const std::vector<QPointF>& points, int currentIndex, int startIndex) const;

    // Переопределяет контрольные точки и узловой вектор кривой по новым узлам, которые необходимо вставить
    Curve redefineControlPointsNodalVectorCurve(Curve &curve) const;
};

