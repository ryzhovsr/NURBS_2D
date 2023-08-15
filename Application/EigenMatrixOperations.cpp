#include "eigenmatrixoperations.h"
#include <Eigen/Dense>

using namespace Eigen;

// Переводит двумерный вектор в матрицу класса Eigen
inline MatrixXd vector2DToMatrix(const IMatrixOperations::vector2D &vec2D)
{
    size_t rows = vec2D.size(), cols = vec2D[0].size();
    MatrixXd matrix = MatrixXd::Constant(rows, cols, 0);

    for (size_t i = 0; i < rows; ++i)
    {
        for (size_t j = 0; j < cols; ++j)
            matrix(i, j) = vec2D[i][j];
    }

    return matrix;
}

// Переводит матрицу класса Eigen в двумерный вектор
inline IMatrixOperations::vector2D matrixToVector2D(const MatrixXd &matrix)
{
    auto rows = matrix.rows(), cols = matrix.cols();
    IMatrixOperations::vector2D vec2D(rows, std::vector<double> (cols));

    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
            vec2D[i][j] = matrix(i, j);
    }

    return vec2D;
}

IMatrixOperations::vector2D EigenMatrixOperations::solveEquation(const vector2D &coefficients, const vector2D &freeMembers)
{
    // Переводим двумерные векторы в матрицу класса Eigen
    MatrixXd coefficientMatrix = vector2DToMatrix(coefficients);
    MatrixXd freeTermMatrix = vector2DToMatrix(freeMembers);

    // Решаем СЛАУ
    Eigen::MatrixXd decisionMatrix = Eigen::MatrixXd::Constant(freeMembers.size(), freeMembers[0].size(), 0);
    decisionMatrix = coefficientMatrix.lu().solve(freeTermMatrix);

    // Revert convertion
    vector2D decisionVector2D = matrixToVector2D(decisionMatrix);

    return decisionVector2D;
}

IMatrixOperations::vector2D EigenMatrixOperations::solveEquation(const vector2D &coefficients, const std::vector<QPointF> &freeMembers)
{
    size_t rows = freeMembers.size(), cols = 2; // cols = 2 потому что всего 2 координаты (x, y)
    vector2D vec2D(rows, std::vector<double> (cols));

    for (int i = 0; i < freeMembers.size(); ++i)
    {
        vec2D[i][0] = freeMembers[i].x(); // Копируем координату x
        vec2D[i][1] = freeMembers[i].y(); // Копируем координату y
    }

    return solveEquation(coefficients, vec2D);
}

int EigenMatrixOperations::getMatrixRank(const vector2D &matrix)
{
    MatrixXd m = vector2DToMatrix(matrix);
    FullPivLU<MatrixXd> lu_decomp(m); // Используем LU-разложение
    return static_cast<int>(lu_decomp.rank());
}
