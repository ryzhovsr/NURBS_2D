#pragma once

#include "IApproxAlgs.h"

class NativeAlg : public IApproxAlgs
{
public:
    Curve approximateCurve(const Curve &curve, int degreeApprox) const override; // Аппрроксимирует кривую до степени degreeApprox
};
