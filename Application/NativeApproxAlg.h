#pragma once

#include "IApproxAlgs.h"

class NativeApproxAlg : public IApproxAlgs
{
public:
    Curve approximateCurve(const Curve &curve, int degreeApprox) const override; // Аппрроксимирует кривую до степени degreeApprox
};

