#pragma once

#include "Curve.h"

class IApproxAlgs
{
public:
    virtual Curve approximateCurve(const Curve &curve, int degreeApprox) const = 0; // Аппрроксимирует кривую до степени degreeApprox

    void checkDegreeApprox(const Curve &curve, int degreeApprox) const;   // Проверяет, что степень исходной кривой < степени аппроксимирующей кривой
};

