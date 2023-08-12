#include "Curve.h"

Curve::Curve(const std::vector<QPointF> &controlPoints, const std::vector<double> &weights, int degree, int curveNumPoints) noexcept
    : _controlPoints {controlPoints}, _weights { weights }, _degree { degree }, _numRealRangePoints { curveNumPoints }
{
    _pointsNURBS.resize(_numRealRangePoints); // Устанавливаем количество точек, из которых будет состоять кривая

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
}

void Curve::calcCurve()
{
    for (int i = 0; i < _numRealRangePoints; ++i) // Итерируемся по каждой точке кривой
    {
        double realPoint = (i / (static_cast<double>(_numRealRangePoints - 1))) * _nodalVector[_realRangeEnd]; // Точка реальной части узл. вектора
        _calcCurvePointAndItsDerivs(_pointsNURBS[i], realPoint); // Рассчитываем точку кривой и её первую и вторую производную
    }
}

void Curve::_calcCurvePointAndItsDerivs(CurvePoint& point, double realPoint)
{
    double span = _findSpanCurve(realPoint); // Вычисляем узловой промежуток для заданной точки (спан)
    point.span = span;
    point.parameter = realPoint;

    // Содержит для заданного узлового вектора ненулевые базис. функции и их производные
    std::vector<std::vector<double>> basisFunctionsAndTheirDerivs(_degree + 1, std::vector<double>(_degree + 1));

    _calcBasisFunctiontsAndTheirDerivs(basisFunctionsAndTheirDerivs, realPoint, span); // Вычисляем ненулевые базисные функции и их производные

    double denominator = 0;
    QPointF n0;

    _calcCurvePoint(basisFunctionsAndTheirDerivs, point, denominator, n0); // Рассчитываем точку кривой

    QPointF n1;
    double n2 = 0;

    calcFirstDerivCurve(basisFunctionsAndTheirDerivs, point, denominator, n0, n1, n2); // Рассчитываем 1-ую производную

    if (_degree == 1) // Если степень полинома = 1, то выходим из функции, чтобы не рассчитывать 2 производную (её не существует в данном случае)
        return;

    calcSecondDerivCurve(basisFunctionsAndTheirDerivs, point, denominator, n0, n1, n2); // Рассчитываем 2-ую производную
}

int Curve::_findSpanCurve(double realPoint)
{
    for (int i = 0; i < _numKnots - 1; ++i) // Проверка на убывание узлового вектора
    {
        if (_nodalVector[i] > _nodalVector[i + 1])
        {
            //qDebug() << "Error! findSpanCurve: узловой вектор убывает nodalVector[i] > nodalVector[i + 1]";
            return {};
        }
    }

    if (_numKnots != (_numVertices + _degree + 1)) // Проверка
    {
        //qDebug() << "Error! findSpanCurve: nodalVector.size()) != (numVertices + degreeCurve + 1)";
        return {};
    }

    if (realPoint < _nodalVector[_degree] || realPoint > _nodalVector[_numKnots - _degree - 1]) // Проверка выхода за реальный диапазон
    {
        //qDebug() << "Error! findSpanCurve: nodalVector вышел за реальный диапазон";
        return {};
    }

    if (realPoint == _nodalVector[_numKnots - _degree - 1]) // Если дошли до конца реального диапазона
        return _numKnots - _degree - 2;

    int low = _degree, high = _numKnots - _degree - 1, middle = (low + high) / 2;

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

void Curve::_calcBasisFunctiontsAndTheirDerivs(std::vector<std::vector<double>>& basisFunctionsAndTheirDerivs, double realPoint, double span)
{
    std::vector<std::vector<double>> tempStorage(_degree + 1, std::vector<double>(_degree + 1)); // Для хранения базисных функций и узлов различия

    calcBasisFuncionsCurve(basisFunctionsAndTheirDerivs, tempStorage, realPoint, span); // Вычисляем ненулевые базиные функции
    calcDerivsBasisFuncionsCurve(basisFunctionsAndTheirDerivs, tempStorage); // Вычисляем производные ненулевых базиных функций

    // Для контроля суммируем значения базисных Функций в точке
    double sum = 0;

    for (size_t i = 0; i < basisFunctionsAndTheirDerivs.size(); ++i)
        sum += basisFunctionsAndTheirDerivs[0][i];

    if ((sum < (1 - 1e-10)) || (sum > 1 + 1e-10)) // Если все верно, то сумма должна = 1
    {
        //qDebug() << "Error! calcBasisFunctiontsAndTheirDerivs: Сумма базисных Функций != 1";
        return;
    }
}

void Curve::_calcCurvePoint(const std::vector<std::vector<double>>& basisFunctionsAndTheirDerivs, CurvePoint& point, double& denominator, QPointF& n0)
{
    for (int i = 0; i < _degree + 1; ++i)
    {
        n0 += _weights[point.span - _degree + i] * _controlPoints[point.span - _degree + i] * basisFunctionsAndTheirDerivs[0][i];
        denominator += basisFunctionsAndTheirDerivs[0][i] * _weights[point.span - _degree + i];
    }

    point.point = n0 / denominator;
}

void Curve::calcFirstDerivCurve(const std::vector<std::vector<double>>& basisFunctionsAndTheirDerivs, CurvePoint& point, double denominator, QPointF &n0, QPointF& n1, double n2)
{
    for (int i = 0; i < _degree + 1; ++i)
    {
        n1 += _weights[point.span - _degree + i] * _controlPoints[point.span - _degree + i] * basisFunctionsAndTheirDerivs[1][i];
        n2 += _weights[point.span - _degree +i] * basisFunctionsAndTheirDerivs[1][i];
    }

    point.firstDeriv = n1 / denominator - (n0 * n2) / (denominator * denominator);
}

void Curve::calcSecondDerivCurve(const std::vector<std::vector<double>>& basisFunctionsAndTheirDerivs, CurvePoint& point, double denominator, QPointF& n0, QPointF& n1, double n2)
{
    QPointF n3;
    double n4 = 0;

    for (int i = 0; i < _degree + 1; ++i)
    {
        n3 += _weights[point.span - _degree + i] * _controlPoints[point.span - _degree + i] * basisFunctionsAndTheirDerivs[2][i];
        n4 += _weights[point.span - _degree +i] * basisFunctionsAndTheirDerivs[2][i];
    }

    QPointF s1 = n3 / denominator - (n1 * n2) / (denominator * denominator);
    QPointF s2 = (n1 * n2 + n0 * n4) / (denominator * denominator) - (n1 * n2 * 2 * n2) / (pow(denominator, 4));

    point.secondDeriv = s1 - s2;
}

void Curve::calcBasisFuncionsCurve(std::vector<std::vector<double>>& basisFunctionsAndTheirDerivs,  std::vector<std::vector<double>>& tempStorage, double realPoint, double span)
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
        basisFunctionsAndTheirDerivs[0][i] = tempStorage[i][_degree];
}

void Curve::calcDerivsBasisFuncionsCurve(std::vector<std::vector<double>>& basisFunctionsAndTheirDerivs, std::vector<std::vector<double>>& tempStorage)
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

            basisFunctionsAndTheirDerivs[k][i] = d;

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
            basisFunctionsAndTheirDerivs[i][j] *= k;

        k *= _degree - i;
    }
}
