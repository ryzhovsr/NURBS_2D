#pragma once

#include "ApproxAlgs.h"

class NativeApproxAlg : public ApproxAlgs
{
public:
    Curve approximateCurve(const Curve &curve, int degreeApprox) const override; // Аппрроксимирует кривую до степени degreeApprox
};

