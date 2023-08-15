#include "Curve.h"
#include <QDebug>

Curve::Curve(const std::vector<QPointF> &controlPoints, const std::vector<double> &weights, int degree, int curveNumPoints) noexcept
    : _controlPoints {controlPoints}, _weights { weights }, _degree { degree }, _numRealRangePoints { curveNumPoints }
{
    _curvePoints.resize(_numRealRangePoints); // Устанавливаем количество точек, из которых будет состоять кривая

    _numVertices = static_cast<int>(_controlPoints.size()); // Кол-во вершин определяющего многоугольника
    _numRealRangeKnots = _numVertices - _degree + 1;        // Кол-во узлов реального диапазона узл. вектора
    _numKnots = _numVertices + _degree + 1;                 // Общее кол-во узлов узл. вектора

    // Начало и конец реального диапазона узл. вектора
    _realRangeStart = _degree;
    _realRangeEnd = _numKnots - _degree - 1;

    double step = 1.0 / (_numRealRangeKnots - 1); // Шаг в реальном диапазоне узл. вектора
    _nodalVector.resize(_numKnots);

    for (int i = _realRangeStart + 1; i < _realRangeEnd; ++i)    // Заполняем реальный диапазон узл. вектора
        _nodalVector[i] = _nodalVector[i - 1] + step;

    for (int i = _realRangeEnd; i < _numKnots; ++i)    // Заполняем оставшуюся часть узл. вектора единицами
        _nodalVector[i] = 1;

    _checkNodalVector();
    calcCurve();
}

void Curve::setNodalVector(const std::vector<double> &nodalVector)
{
    _nodalVector = nodalVector;
    _checkNodalVector();
}

std::vector<CurvePoint> Curve::getCurvePoints() const noexcept
{
    return _curvePoints;
}

std::vector<QPointF> Curve::getControlPoints() const noexcept
{
    return _controlPoints;
}

std::vector<double> Curve::getNodalVector() const noexcept
{
    return _nodalVector;
}

std::vector<double> Curve::getWeights() const noexcept
{
    return _weights;
}

int Curve::getDegree() const noexcept
{
    return _degree;
}

void Curve::_checkNodalVector()
{
    for (int i = 0; i < _numKnots - 1; ++i) // Проверка узлового вектора на убывание
    {
        if (_nodalVector[i] > _nodalVector[i + 1])
            qDebug() << "Error! _checkNodalVector: узловой вектор убывает _nodalVector[i] > _nodalVector[i + 1]!";
    }

    if (_numKnots != (_numVertices + _degree + 1))
        qDebug() << "Error! _checkNodalVector: nodalVector.size()) != (numVertices + degreeCurve + 1)!";
}

void Curve::calcCurve()
{
    for (int i = 0; i < _numRealRangePoints; ++i) // Итерируемся по каждой точке кривой
    {
        // Находим точку реальной части узл. вектора (параметр кривой ∈ [0, 1])
        double realPoint = static_cast<double>(i) / (_numRealRangePoints - 1) * (_nodalVector[_realRangeEnd] - _nodalVector[_realRangeStart]) + _nodalVector[_realRangeStart];
        _calcCurvePointAndDerivs(_curvePoints[i], realPoint); // Рассчитываем точку кривой и её первую и вторую производную
    }
}

void Curve::_calcCurvePointAndDerivs(CurvePoint &curvePoint, double realPoint)
{
    curvePoint.parameter = realPoint;
    curvePoint.span = _findSpanForRealPoint(realPoint); // Вычисляем узловой промежуток (спан) для заданной точки

    std::vector<std::vector<double>> basisFuncsAndTheirDerivs = _calcBasisFuncsAndTheirDerivs(realPoint, curvePoint.span); // Вычисляем базисные функции и их производные

    double denominator = 0;
    QPointF n0;

    calcPointCurve(basisFuncsAndTheirDerivs, curvePoint, denominator, n0); // Рассчитываем точку кривой

    QPointF n1;
    double n2 = 0;

    _calcPointFirstDeriv(basisFuncsAndTheirDerivs, curvePoint, denominator, n0, n1, n2); // Рассчитываем 1-ую производную

    if (_degree == 1) // Если степень кривой = 1, то выходим из функции
        return;

    _calcPointSecondDeriv(basisFuncsAndTheirDerivs, curvePoint, denominator, n0, n1, n2); // Рассчитываем 2-ую производную
}

int Curve::_findSpanForRealPoint(double realPoint)
{
    if (realPoint < _nodalVector[_degree] || realPoint > _nodalVector[_numKnots - _degree - 1]) // Если realPoint выходит за реальный диапазон узл. вектора
    {
        qDebug() << "Error! _findSpanForRealPoint: nodalVector вышел за реальный диапазон!";
        return -1;
    }

    if (realPoint == _nodalVector[_numKnots - _degree - 1]) // Если дошли до конца реального диапазона узл. вектора
        return _numKnots - _degree - 2;

    int low = _degree;
    int high = _numKnots - _degree - 1;
    int middle = (low + high) / 2;

    while ((realPoint < _nodalVector[middle]) || (realPoint >= _nodalVector[middle + 1]))
    {
        if (realPoint < _nodalVector[middle])
            high = middle;
        else
            low = middle;

        middle = (low + high) / 2;
    }

    return middle;
}

std::vector<std::vector<double>> Curve::_calcBasisFuncsAndTheirDerivs(double realPoint, double span)
{
    std::vector<std::vector<double>> basisFuncsAndTheirDerivs(_degree + 1, std::vector<double>(_degree + 1));
    std::vector<std::vector<double>> tempStorage(_degree + 1, std::vector<double>(_degree + 1));    // Для хранения базисных функций и узлов различия

    _calcBasisFuncs(basisFuncsAndTheirDerivs, tempStorage, realPoint, span);    // Вычисляем базиные функции
    _calcDerivsBasisFuncs(basisFuncsAndTheirDerivs, tempStorage);               // Вычисляем производные базиных функций

    double sum = 0;

    for (int i = 0; i < _degree + 1; ++i)   // Для контроля суммируем значения базисных функций в точке
        sum += basisFuncsAndTheirDerivs[0][i];

    if (sum < (1 - 1e-10) || (sum > 1 + 1e-10)) // Если сумма базисных функций не равна 1
        qDebug() << "Error! _calcBasisFuncsAndTheirDerivs: Сумма базисных функций != 1!";

    return basisFuncsAndTheirDerivs;
}

void Curve::_calcBasisFuncs(std::vector<std::vector<double>> &basisFuncsAndTheirDerivs, std::vector<std::vector<double>> &tempStorage, double realPoint, double span)
{
    std::vector<double> left(_degree + 1), right(_degree + 1);
    tempStorage[0][0] = 1;

    for (int i = 1; i < _degree + 1; ++i)
    {
        left[i] = realPoint - _nodalVector[span + 1 - i];
        right[i] = _nodalVector[span + i] - realPoint;
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

    for (int i = 0; i <= _degree; ++i)
        basisFuncsAndTheirDerivs[0][i] = tempStorage[i][_degree];
}

void Curve::_calcDerivsBasisFuncs(std::vector<std::vector<double>> &basisFuncsAndTheirDerivs, std::vector<std::vector<double>> &tempStorage)
{
    std::vector<std::vector<double>> rows(2, std::vector<double>(_degree + 1)); // Хранит два вычесленных ряда

    for (int i = 0; i < _degree + 1; ++i)
    {
        int s1 = 0, s2 = 1; // Альтернативные строки в массиве
        rows[0][0] = 1;

        for (int k = 1; k <= _degree; ++k)
        {
            double d = 0;
            double rk = i - k;
            double pk = _degree - k;

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
                j2 = _degree - i;

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

    double k = _degree;

    for (int i = 1; i <= _degree; ++i) // Умножаем на правильные коэффициенты
    {
        for (int j = 0; j < _degree + 1; ++j)
            basisFuncsAndTheirDerivs[i][j] *= k;

        k *= _degree - i;
    }
}

void Curve::calcPointCurve(const std::vector<std::vector<double>> &basisFunctionsAndTheirDerivs, CurvePoint &curvePoint, double &denominator, QPointF &n0)
{
    for (int i = 0; i < _degree + 1; ++i)
    {
        n0 += _weights[curvePoint.span - _degree + i] * _controlPoints[curvePoint.span - _degree + i] * basisFunctionsAndTheirDerivs[0][i];
        denominator += basisFunctionsAndTheirDerivs[0][i] * _weights[curvePoint.span - _degree + i];
    }

    curvePoint.point = n0 / denominator;
}

void Curve::_calcPointFirstDeriv(const std::vector<std::vector<double>> &basisFuncsAndTheirDerivs, CurvePoint &curvePoint, double denominator, QPointF &n0, QPointF &n1, double n2)
{
    for (int i = 0; i < _degree + 1; ++i)
    {
        n1 += _weights[curvePoint.span - _degree + i] * _controlPoints[curvePoint.span - _degree + i] * basisFuncsAndTheirDerivs[1][i];
        n2 += _weights[curvePoint.span - _degree +i] * basisFuncsAndTheirDerivs[1][i];
    }

    curvePoint.firstDeriv = n1 / denominator - (n0 * n2) / (denominator * denominator);
}

void Curve::_calcPointSecondDeriv(const std::vector<std::vector<double>> &basisFuncsAndTheirDerivs, CurvePoint &curvePoint, double denominator, QPointF &n0, QPointF &n1, double n2)
{
    QPointF n3;
    double n4 = 0;

    for (int i = 0; i < _degree + 1; ++i)
    {
        n3 += _weights[curvePoint.span - _degree + i] * _controlPoints[curvePoint.span - _degree + i] * basisFuncsAndTheirDerivs[2][i];
        n4 += _weights[curvePoint.span - _degree +i] * basisFuncsAndTheirDerivs[2][i];
    }

    QPointF s1 = n3 / denominator - (n1 * n2) / (denominator * denominator);
    QPointF s2 = (n1 * n2 + n0 * n4) / (denominator * denominator) - (n1 * n2 * 2 * n2) / (pow(denominator, 4));

    curvePoint.secondDeriv = s1 - s2;
}
