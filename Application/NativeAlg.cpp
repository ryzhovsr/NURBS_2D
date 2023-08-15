#include "NativeAlg.h"

Curve NativeAlg::approximateCurve(const Curve &curve, int degreeApprox) const
{
    checkDegreeApprox(curve, degreeApprox);
    return Curve(curve.getControlPoints(), curve.getWeights(), degreeApprox, static_cast<int>(curve.getCurvePoints().size()));
}
