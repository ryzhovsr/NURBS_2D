#pragma once

#include <vector>
#include <memory>
#include <QPointF>

class IMatrixOperations;
using IMatrixOperationsPtr = std::shared_ptr<IMatrixOperations>;

enum OperationClass // Содержит названия библиотек для работы с матрицами
{
    eigen
};

class IMatrixOperations
{
public:
    using vector2D = std::vector<std::vector<double>>;

    // Решает СЛАУ
    virtual vector2D solveEquation(const vector2D& coefficients, const vector2D& freeMembers) = 0;
    virtual vector2D solveEquation(const vector2D& coefficients, const std::vector<QPointF>& freeMembers) = 0;

    // Возвращяет ранг матрицы
    virtual int getMatrixRank(const vector2D& matrix) = 0;

    static IMatrixOperationsPtr GetMatrixOperationsClass (OperationClass className);
};
