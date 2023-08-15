#pragma once

#include "imatrixoperations.h"

class EigenMatrixOperations final : public IMatrixOperations
{
public:
    // Решает СЛАУ
    vector2D solveEquation(const vector2D &coefficients, const vector2D &freeMembers) override;
    vector2D solveEquation(const vector2D &coefficients, const std::vector<QPointF> &freeMembers) override;

    // Возвращает ранг матрицы
    int getMatrixRank(const vector2D &matrix) override;
};
