#include "ApproxAlgs.h"
#include <QDebug>

void ApproxAlgs::checkDegreeApprox(const Curve &curve, int degreeApprox) const
{
    if (curve.getDegree() < degreeApprox)
        qDebug() << "Error! checkDegreeApprox: Степень исходной кривой больше степени аппроксимирующей кривой!";
}
