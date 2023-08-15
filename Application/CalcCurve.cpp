#include "CalcCurve.h"
#include <QDebug>

void CalcCurve::calcCurvePointAndDerivs(const Curve &curve, CurvePoint &curvePoint, double parameter)
{
    curvePoint.parameter = parameter;
    int degree = curve.getDegree();
    curvePoint.span = findSpanForParameter(parameter, curve.getNodalVector(), degree); // Вычисляем узловой промежуток (спан) для заданного параметра

    std::vector<std::vector<double>> basisFuncsAndTheirDerivs = calcBasisFuncsAndTheirDerivs(curve.getNodalVector(), parameter, curvePoint.span, degree); // Вычисляем базисные функции и их производные

    double denominator = 0;
    QPointF n0;

    calcPointCurve(curve, basisFuncsAndTheirDerivs, curvePoint, denominator, n0); // Рассчитываем точку кривой

    QPointF n1;
    double n2 = 0;

    calcPointFirstDeriv(curve, basisFuncsAndTheirDerivs, curvePoint, denominator, n0, n1, n2); // Рассчитываем 1-ую производную

    if (curve.getDegree() == 1) // Если степень кривой = 1, то выходим из функции
        return;

    calcPointSecondDeriv(curve, basisFuncsAndTheirDerivs, curvePoint, denominator, n0, n1, n2); // Рассчитываем 2-ую производную
}

int CalcCurve::findSpanForParameter(double parameter, const std::vector<double> &nodalVector, int degree)
{
    int numKnots = static_cast<int>(nodalVector.size());

    if (parameter < nodalVector[degree] || parameter > nodalVector[numKnots - degree - 1]) // Если parameter выходит за реальный диапазон узл. вектора
    {
        qDebug() << "Error! findSpanForParameter: parameter вышел за реальный диапазон!";
        return -1;
    }

    if (parameter == nodalVector[numKnots - degree - 1]) // Если дошли до конца реального диапазона узл. вектора
        return numKnots - degree - 2;

    int low = degree;
    int high = numKnots - degree - 1;
    int middle = (low + high) / 2;

    while ((parameter < nodalVector[middle]) || (parameter >= nodalVector[middle + 1]))
    {
        if (parameter < nodalVector[middle])
            high = middle;
        else
            low = middle;

        middle = (low + high) / 2;
    }

    return middle;
}

std::vector<std::vector<double>> CalcCurve::calcBasisFuncsAndTheirDerivs(const std::vector<double>& nodalVector, double parameter, int span, int degree)
{
    std::vector<std::vector<double>> basisFuncsAndTheirDerivs(degree + 1, std::vector<double>(degree + 1));
    std::vector<std::vector<double>> tempStorage(degree + 1, std::vector<double>(degree + 1));    // Для хранения базисных функций и узлов различия

    calcBasisFuncs(basisFuncsAndTheirDerivs, tempStorage, nodalVector, parameter, span, degree);  // Вычисляем базиные функции
    calcDerivsBasisFuncs(basisFuncsAndTheirDerivs, tempStorage, degree);            // Вычисляем производные базиных функций

    double sum = 0;

    for (int i = 0; i < degree + 1; ++i)   // Для контроля суммируем значения базисных функций в точке
        sum += basisFuncsAndTheirDerivs[0][i];

    if (sum < (1 - 1e-10) || (sum > 1 + 1e-10)) // Если сумма базисных функций не равна 1
        qDebug() << "Error! calcBasisFuncsAndTheirDerivs: Сумма базисных функций != 1!";

    return basisFuncsAndTheirDerivs;
}

void CalcCurve::calcBasisFuncs(std::vector<std::vector<double>> &basisFuncsAndTheirDerivs, std::vector<std::vector<double>> &tempStorage, const std::vector<double>& nodalVector, double parameter, int span, int degree)
{
    std::vector<double> left(degree + 1), right(degree + 1);
    tempStorage[0][0] = 1;

    for (int i = 1; i < degree + 1; ++i)
    {
        left[i] = parameter - nodalVector[span + 1 - i];
        right[i] = nodalVector[span + i] - parameter;
        double saved = 0;

        for (int j = 0; j < i; ++j)
        {
            // Нижний треугольник
            tempStorage[i][j] = right[j + 1] + left[i - j];
            double temp = tempStorage[j][i - 1] / tempStorage[i][j];
            // Верхний треугольник
            tempStorage[j][i] = saved + right[j + 1] * temp;
            saved = left[i - j] * temp;
        }

        tempStorage[i][i] = saved;
    }

    for (int i = 0; i <= degree; ++i)
        basisFuncsAndTheirDerivs[0][i] = tempStorage[i][degree];
}

void CalcCurve::calcDerivsBasisFuncs(std::vector<std::vector<double>> &basisFuncsAndTheirDerivs, std::vector<std::vector<double>> &tempStorage, int degree)
{
    std::vector<std::vector<double>> rows(2, std::vector<double>(degree + 1)); // Хранит два вычесленных ряда

    for (int i = 0; i < degree + 1; ++i)
    {
        int s1 = 0, s2 = 1; // Альтернативные строки в массиве
        rows[0][0] = 1;

        for (int k = 1; k <= degree; ++k)
        {
            double d = 0;
            double rk = i - k;
            double pk = degree - k;

            if (i >= k)
            {
                rows[s2][0] = rows[s1][0] / tempStorage[pk + 1][rk];
                d = rows[s2][0] * tempStorage[rk][pk];
            }

            double j1 = 0;

            if (rk >= -1)
                j1 = 1;
            else
                j1 = -rk;

            double j2 = 0;

            if (i - 1 <= pk)
                j2 = k - 1;
            else
                j2 = degree - i;

            for (int j = j1; j <= j2; ++j)
            {
                rows[s2][j] = (rows[s1][j] - rows[s1][j - 1]) / tempStorage[pk + 1][rk + j];
                d += rows[s2][j] * tempStorage[rk + j][pk];
            }

            if (i <= pk)
            {
                rows[s2][k] = -rows[s1][k - 1] / tempStorage[pk + 1][i];
                d += rows[s2][k] * tempStorage[i][pk];
            }

            basisFuncsAndTheirDerivs[k][i] = d;

            // Меняем строки местами
            int temp = s1;
            s1 = s2;
            s2 = temp;
        }
    }

    double k = degree;

    for (int i = 1; i <= degree; ++i) // Умножаем на правильные коэффициенты
    {
        for (int j = 0; j < degree + 1; ++j)
            basisFuncsAndTheirDerivs[i][j] *= k;

        k *= degree - i;
    }
}

void CalcCurve::calcPointCurve(const Curve &curve, const std::vector<std::vector<double>> &basisFunctionsAndTheirDerivs, CurvePoint &curvePoint, double &denominator, QPointF &n0)
{
    int degree = curve.getDegree();
    std::vector<double> weights = curve.getWeights();
    std::vector<QPointF> controlPoints = curve.getControlPoints();

    for (int i = 0; i < degree + 1; ++i)
    {
        n0 += basisFunctionsAndTheirDerivs[0][i] * weights[curvePoint.span - degree + i] * controlPoints[curvePoint.span - degree + i];
        denominator += basisFunctionsAndTheirDerivs[0][i] * weights[curvePoint.span - degree + i];
    }

    curvePoint.point = n0 / denominator;
}

void CalcCurve::calcPointFirstDeriv(const Curve &curve, const std::vector<std::vector<double>> &basisFuncsAndTheirDerivs, CurvePoint &curvePoint, double denominator, QPointF &n0, QPointF &n1, double n2)
{
    int degree = curve.getDegree();
    std::vector<double> weights = curve.getWeights();
    std::vector<QPointF> controlPoints = curve.getControlPoints();

    for (int i = 0; i < degree + 1; ++i)
    {
        n1 += basisFuncsAndTheirDerivs[1][i] * weights[curvePoint.span - degree + i] * controlPoints[curvePoint.span - degree + i] ;
        n2 += basisFuncsAndTheirDerivs[1][i] * weights[curvePoint.span - degree + i];
    }

    curvePoint.firstDeriv = n1 / denominator - (n0 * n2) / (denominator * denominator);
}

void CalcCurve::calcPointSecondDeriv(const Curve &curve, const std::vector<std::vector<double>> &basisFuncsAndTheirDerivs, CurvePoint &curvePoint, double denominator, QPointF &n0, QPointF &n1, double n2)
{
    int degree = curve.getDegree();
    std::vector<double> weights = curve.getWeights();
    std::vector<QPointF> controlPoints = curve.getControlPoints();

    QPointF n3;
    double n4 = 0;

    for (int i = 0; i < degree + 1; ++i)
    {
        n3 += weights[curvePoint.span - degree + i] * basisFuncsAndTheirDerivs[2][i] * controlPoints[curvePoint.span - degree + i];
        n4 += weights[curvePoint.span - degree + i] * basisFuncsAndTheirDerivs[2][i];
    }

    QPointF s1 = n3 / denominator - (n1 * n2) / (denominator * denominator);
    QPointF s2 = (n1 * n2 + n0 * n4) / (denominator * denominator) - (n1 * n2 * 2 * n2) / (pow(denominator, 4));

    curvePoint.secondDeriv = s1 - s2;
}
