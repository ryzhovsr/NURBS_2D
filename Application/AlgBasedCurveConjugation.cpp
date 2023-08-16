#include "AlgBasedCurveConjugation.h"
#include <QDebug>
#include <CalcCurve.h>
#include "MathUtils.h"
#include "IMatrixOperations.h"

Curve AlgBasedCurveConjugation::approximateCurve(const Curve &curve, int degreeApprox) const
{ // СТЕПЕНЬ ИСХОДНОЙ КРИВОЙ ДОЛЖНА БЫТЬ НА 2 МЕНЬШЕ
    Curve tempCurve = curve;
    int degree = tempCurve.getDegree();
    int numParameters = static_cast<int>(tempCurve.getCurvePoints().size());

    addNodalPoints(tempCurve);

    std::vector<QPointF> tempControlPoints = tempCurve.getControlPoints();

    std::vector<QPointF> controlPointsBezier_1(tempControlPoints.begin(), tempControlPoints.end() -  degree);
    std::vector<QPointF> controlPointsBezier_2(tempControlPoints.begin() + degree, tempControlPoints.end());
    std::vector<double> newWeights(controlPointsBezier_1.size(), 1);

    Curve bezier1(controlPointsBezier_1, newWeights, degree, numParameters);
    Curve bezier2(controlPointsBezier_2, newWeights, degree, numParameters);

    reductBezierCurveDegree(bezier1);
    reductBezierCurveDegree(bezier2);
    tempCurve = attachCurve(bezier1, bezier2, true);

    Curve resCurve = tempCurve;

    for (int currentDegree = tempCurve.getDegree(); currentDegree > degreeApprox; --currentDegree)
    {
        tempCurve = addMultipleKnot(tempCurve); // Добавляем кратный узел в кривую

        tempControlPoints = tempCurve.getControlPoints();
        std::vector<QPointF> tempControlPoints_1(tempControlPoints.begin(), tempControlPoints.end() -  tempCurve.getDegree());
        std::vector<QPointF> tempControlPoints_2(tempControlPoints.begin() + tempCurve.getDegree(), tempControlPoints.end());
        std::vector<double> tempWeights(controlPointsBezier_1.size(), 1);
        Curve tempBezier_1(tempControlPoints_1, tempWeights, currentDegree, numParameters);
        Curve tempBezier_2(tempControlPoints_2, tempWeights, currentDegree, numParameters);

        reductBezierCurveDegree(tempBezier_1);
        reductBezierCurveDegree(tempBezier_2);
        tempCurve = attachCurve(tempBezier_1, tempBezier_2, true);
        resCurve = tempCurve;
    }

    return resCurve;
}

void AlgBasedCurveConjugation::addNodalPoints(Curve &curve) const
{
    int degree = curve.getDegree();
    int numRealRangeKnots = static_cast<int>(curve.getControlPoints().size()) - degree + 1; // Кол-во узлов реального диапазона узл. вектора
    std::vector<double> nodalVector = curve.getNodalVector();
    std::vector<QPointF> controlPoints = curve.getControlPoints();
    int numNewNodes = (degree - 1) * (numRealRangeKnots - 2); // "-1" не берём уже имеющиеся узлы; "-2" не берём граничные узлы равные 0 и 1
    std::vector<double> newNodes;
    int numKnots = static_cast<int>(nodalVector.size());

    // Начало и конец реального диапазона узл. вектора
    int realRangeStart = degree;
    int realRangeEnd = numKnots - degree - 1;

    int numNotBoundaryNodals = 0; // Количество не граничных узлов (не равных 0 и 1)

    for (int i = realRangeStart; i < realRangeEnd; ++i)
    {
        if (nodalVector[i] != 0 && nodalVector[i] != 1) // Пропускаем границы реального диапазона
            ++numNotBoundaryNodals;
    }

    if (numNotBoundaryNodals == 0)
    {
        qDebug() << "Error! addNodalPoints: numNotBoundaryNodals == 0";
        return;
    }

    for (int i = realRangeStart; i < realRangeEnd; ++i)
    {
        int counter = 0;  // Кол-во добавлений нового узла

        while (counter < numNewNodes / numNotBoundaryNodals)
        {
            if (nodalVector[i] == 0 || nodalVector[i] == 1) // Пропускаем границы реального диапазона
                break;

            newNodes.push_back(nodalVector[i]);;
            ++counter;
        }
    }

    int numOldControlPoints = static_cast<int>(curve.getControlPoints().size());    // Коли-во контрольных точек до вставки
    int maxIndexNewNodes = static_cast<int>(newNodes.size() - 1);         // Максимальный индекс вектора newNodes

    std::vector<QPointF> newControlPoints(numOldControlPoints + newNodes.size()); // Новые контрольные точки
    std::vector<double> newNodalVector(nodalVector.size() + newNodes.size());  // Новый узловой вектор

    double a = CalcCurve::findSpanForParameter(newNodes[0], nodalVector, degree);
    double b = CalcCurve::findSpanForParameter(newNodes[maxIndexNewNodes], nodalVector, degree) + 1;

    for (int i = 0; i < a - degree + 1; ++i)
        newControlPoints[i] = controlPoints[i];

    for (int i = b - 1; i < numOldControlPoints; ++i)
        newControlPoints[i + maxIndexNewNodes + 1] = controlPoints[i];

    for (int i = 0; i < a + 1; ++i)
        newNodalVector[i] = nodalVector[i];

    for (int i = b + degree; i < numOldControlPoints + degree + 1; ++i)
        newNodalVector[i + maxIndexNewNodes + 1] = nodalVector[i];

    int ind = b + degree - 1;
    int k = b + degree + maxIndexNewNodes;

    for (int i = maxIndexNewNodes; i > -1; --i)
    {
        while ((newNodes[i] <= nodalVector[ind]) && (ind > a))
        {
            newControlPoints[k - degree - 1] = controlPoints[ind - degree - 1];
            newNodalVector[k] = nodalVector[ind];
            --k, --ind;
        }

        newControlPoints[k - degree - 1] = newControlPoints[k - degree];

        for (int j = 1; j < degree + 1; ++j)
        {
            int temp = k - degree + j;
            double alpha = newNodalVector[k + j] - newNodes[i];

            if (alpha == 0)
                newControlPoints[temp - 1] = newControlPoints[temp];
            else
            {
                alpha /= (newNodalVector[k + j] - nodalVector[ind - degree + j]);
                newControlPoints[temp - 1].setX(alpha * newControlPoints[temp - 1].x() + (1 - alpha) * newControlPoints[temp].x());
                newControlPoints[temp - 1].setY(alpha * newControlPoints[temp - 1].y() + (1 - alpha) * newControlPoints[temp].y());
            }
        }

        newNodalVector[k] = newNodes[i];
        --k;
    }

    curve = redefineCurve(newControlPoints, newNodalVector, degree, static_cast<int>(curve.getCurvePoints().size()));
}

Curve AlgBasedCurveConjugation::redefineCurve(const std::vector<QPointF>& controlPoints, const std::vector<double>& nodalVector, int degree, int numParameters) const
{
    std::vector<double> weights(controlPoints.size(), 1);

    Curve newCurve(controlPoints, weights, degree, numParameters);

    if (!nodalVector.empty()) // Если новый узловой вектор пустой, то заполним его равномерно
    {
        newCurve.setNodalVector(nodalVector);
        newCurve.calcCurve();
    }

    return newCurve;
}

void AlgBasedCurveConjugation::reductBezierCurveDegree(Curve &curve) const
{
    std::vector<QPointF> controlPoints = curve.getControlPoints();
    int degree = curve.getDegree();

    if (controlPoints.size() - 1 != degree) // Проверка что сплайн является кривой Безье
    {
        qDebug() << "Error! reductBezierCurveDegree: максимальный индекс массива не равен степени _controlPoints.size() != _degree";
        return;
    }

    if (degree <= 2)
    {
        qDebug() << "Error! reductBezierCurveDegree: порядок исходного сплайна не может быть меньше 3";
        return;
    }

    int numOldControlPoints = static_cast<int>(controlPoints.size());
    std::vector<QPointF> newControlPoints(numOldControlPoints - 1); // Новые контрольные точки

    // Присваиваем коор-ты первой и последней старых контрольных точек
    newControlPoints[0] = controlPoints[0];
    newControlPoints[numOldControlPoints - 2] = controlPoints[numOldControlPoints - 1];

    int meadle = (degree - 1) / 2;

    // Заполняем новые контрольные точки
    for (int i = 1; i < meadle + 1; ++i)
        newControlPoints[i] = (controlPoints[i] - (i / static_cast<double>(degree)) * newControlPoints[i - 1]) / (1 - (i / static_cast<double>(degree)));

    for (int i = degree - 2; i > meadle; --i)
        newControlPoints[i] = (controlPoints[i + 1] - (1 - (i + 1) / static_cast<double>(degree)) * newControlPoints[i + 1]) / ((i + 1) / static_cast<double>(degree));

    if (degree % 2 != 0) // Для нечётной степени вычисляем центральную контрольную точку
    {
        QPointF left = (controlPoints[meadle] - (meadle / static_cast<double>(degree)) * newControlPoints[meadle - 1]) / (1 - (meadle / static_cast<double>(degree)));

        QPointF right = (controlPoints[meadle + 1] - (1 - (meadle + 1) / static_cast<double>(degree)) * newControlPoints[meadle + 1]) / ((meadle + 1) / static_cast<double>(degree));

        newControlPoints[meadle] = (left + right) * 0.5;
    }

    curve = redefineCurve(newControlPoints, {}, --degree, static_cast<int>(curve.getCurvePoints().size()));
}

Curve AlgBasedCurveConjugation::attachCurve(const Curve &curve1, const Curve &curve2, bool fixateStartEndPoints) const
{
    std::vector<QPointF> controlPointsFirstCurve = curve1.getControlPoints();
    std::vector<QPointF> controlPointsSecondCurve = curve2.getControlPoints();
    int numControlPoints; // Кол-во контрольных точек

    if (curve1.getControlPoints().size() != curve2.getControlPoints().size()) // Проверка на одинаковое кол-во контрольных точек кривых (степень кривых Безье должны быть равны)
        qDebug() << "Error! attachCurvesUsualMethod: кривые Безье имеют разное кол-во контрольных точек";
    else
        numControlPoints = static_cast<int>(curve1.getControlPoints().size());

    std::vector<QPointF> derivsFirstCurve(numControlPoints);    // Производные первой кривой
    std::vector<QPointF> negDerivsFirstCurve(numControlPoints); // Отрицательные дельты контрольных точек первой кривой
    int startIndexFirstCurve = numControlPoints - 1;          // Стартовый индекс начинается с конца

    for (int i = numControlPoints - 1; i >= 0 ; --i) // Находим дельты для первой кривой
    {
        derivsFirstCurve[numControlPoints - i - 1] = calcDerivLeftBezierCurveForMerger(controlPointsFirstCurve, i, startIndexFirstCurve);
        negDerivsFirstCurve[numControlPoints - i - 1] = calcNegativeDerivLeftBezierCurveForMerger(controlPointsFirstCurve, i, startIndexFirstCurve);
    }

    std::vector<QPointF> derivsSecondCurve(numControlPoints);   // Производные второй кривой
    int startIndexSecondCurve = 0;    // Стартовый индекс начинается с начала

    for (int i = 0; i < numControlPoints; ++i)    // Находим дельты для второй кривой
        derivsSecondCurve[i] = calcDerivRightBezierCurveForMerger(controlPointsSecondCurve, i, startIndexSecondCurve);

    std::vector<std::vector<double>> coefficients(numControlPoints * 3, std::vector<double> (numControlPoints * 3)); // Матрица коэффициентов

    // Заполняем матрицу коэффициентов
    for (int i = 0; i < numControlPoints * 2; ++i)
        coefficients[i][i] = 2;

    for (int i = 0; i < numControlPoints; ++i)
    {
        int resNum = 0;
        int counter = 0;

        for (int r = 0; r <= i; ++r)
        {
            resNum = pow(-1, r - i) * MathUtils::calcCombWithoutRepetition(i, r);
            coefficients[numControlPoints * 2 + i][startIndexFirstCurve - i + counter] = resNum;
            coefficients[numControlPoints * 2 + i][startIndexFirstCurve + i - counter + 1] = pow(-1, i % 2? 0 : 1) * resNum;

            coefficients[startIndexFirstCurve - i + counter][numControlPoints * 2 + i] = resNum;
            coefficients[startIndexFirstCurve + i - counter + 1][numControlPoints * 2 + i] = pow(-1, i % 2? 0 : 1) * resNum;
            ++counter;
        }
    }

    if (fixateStartEndPoints) // Если начальная и конечная точка фиксированная
    { // Изменяем матрицу коэффициентов
        coefficients[0][0] = 1;
        coefficients[numControlPoints * 2 - 1][numControlPoints * 2 - 1] = 1;
        coefficients[0][coefficients[0].size() - 1] = 0;
        coefficients[numControlPoints * 2 - 1][coefficients[0].size() - 1] = 0;
    }

    std::vector<QPointF> freeMembers(numControlPoints * 3); // Матрица свободных членов
    int counter = 0;

    for (int i = numControlPoints * 2; i < numControlPoints * 3; ++ i) // Заполняем матрицу свободных членов
    {
        freeMembers[i] = negDerivsFirstCurve[counter] + derivsSecondCurve[counter];
        ++counter;
    }

    auto operation = IMatrixOperations::GetMatrixOperationsClass(OperationClass::eigen); // Создаём указатель на интерфейс операций СЛАУ

    if (operation == nullptr)
        qDebug () << "Error! attachCurvesUsualMethod: oper == nullptr";;

    auto solution = operation->solveEquation(coefficients, freeMembers); // Решаем СЛАУ

    std::vector<QPointF> tempPoints(numControlPoints); // Временные точки для рассчёта новых контрольных точек
    std::vector<QPointF> controlPointsNewCurve(numControlPoints);

    for (int i = 0; i < numControlPoints; ++i)
    {
        tempPoints[i].setX(controlPointsFirstCurve[i].x() + solution[i][0]);
        tempPoints[i].setY(controlPointsFirstCurve[i].y() + solution[i][1]);
    }

    controlPointsNewCurve[0] = tempPoints[0];
    counter = 1;

    while (counter != numControlPoints)
    {
        for (int i = 0; i < numControlPoints - counter; ++i)
            tempPoints[i] =  -1 * tempPoints[i] + 2 * tempPoints[i + 1];

        controlPointsNewCurve[counter] = tempPoints[0];
        ++counter;
    }

    // Найдём новые точки двух кривых по отдельности
    std::vector<QPointF> newControlPointsFirstCurve(numControlPoints);
    std::vector<QPointF> newControlPointsSecondCurve(numControlPoints);

    for (int i = 0; i < numControlPoints; ++i)
    {
        newControlPointsFirstCurve[i].setX(controlPointsFirstCurve[i].x() + solution[i][0]);
        newControlPointsFirstCurve[i].setY(controlPointsFirstCurve[i].y() + solution[i][1]);
        newControlPointsSecondCurve[i].setX(controlPointsSecondCurve[i].x() + solution[numControlPoints + i][0]);
        newControlPointsSecondCurve[i].setY(controlPointsSecondCurve[i].y() + solution[numControlPoints + i][1]);
    }

    //this->_controlPoints = newControlPoints_P; // Если нужно, можем присвоить новые точки текущим кривым
    //anotherCurve._controlPoints=newControlPoints_Q;

    for (int i = numControlPoints - 1; i >= 0 ; --i) // Находим новые производные для первой кривой
    {
        derivsFirstCurve[numControlPoints - i - 1] = calcDerivLeftBezierCurveForMerger(newControlPointsFirstCurve, i, startIndexFirstCurve);
        negDerivsFirstCurve[numControlPoints - i - 1] = calcNegativeDerivLeftBezierCurveForMerger(newControlPointsFirstCurve, i, startIndexFirstCurve);
    }

    for (int i = 0; i < numControlPoints; ++i)   // Находим новые производные для второй кривой
        derivsSecondCurve[i] = calcDerivRightBezierCurveForMerger(newControlPointsSecondCurve, i, startIndexSecondCurve);

    for (int i = 0; i < numControlPoints; ++i)
    {
        if (abs(derivsFirstCurve[i].x() - derivsSecondCurve[i].x()) > 0.001 || abs(derivsFirstCurve[i].y() - derivsSecondCurve[i].y()) > 0.001)
            qDebug() << "Error! attachCurvesUsualMethod: derivsFirstCurve[i] != derivsSecondCurve[i]";
    }

    return Curve(controlPointsNewCurve, curve1.getWeights(), curve1.getDegree(), static_cast<int>(curve1.getCurvePoints().size()));
}

QPointF AlgBasedCurveConjugation::calcDerivLeftBezierCurveForMerger(const std::vector<QPointF>& points, int currentIndex, int startIndex) const
{
    if(startIndex == currentIndex)
        return points[currentIndex];
    else
        return calcDerivLeftBezierCurveForMerger(points, currentIndex + 1, startIndex) - calcDerivLeftBezierCurveForMerger(points, currentIndex, startIndex - 1);
}

QPointF AlgBasedCurveConjugation::calcNegativeDerivLeftBezierCurveForMerger(const std::vector<QPointF>& points, int currentIndex, int startIndex) const
{
    if(startIndex == currentIndex)
        return -1 * points[currentIndex];
    else
        return calcNegativeDerivLeftBezierCurveForMerger(points, currentIndex + 1, startIndex)  - calcNegativeDerivLeftBezierCurveForMerger(points, currentIndex, startIndex - 1);
}

QPointF AlgBasedCurveConjugation::calcDerivRightBezierCurveForMerger(const std::vector<QPointF>& points, int currentIndex, int startIndex) const
{
    if(startIndex == currentIndex)
        return points[currentIndex];
    else
        return calcDerivRightBezierCurveForMerger(points, currentIndex, startIndex + 1) - calcDerivRightBezierCurveForMerger(points, currentIndex - 1, startIndex);
}

Curve AlgBasedCurveConjugation::addMultipleKnot(Curve &curve) const
{
    int degree = curve.getDegree();
    std::vector<double> newNodes(degree, 0.5); // Новый вектор узловых точек, который будет добавлен в текущий
    std::vector<QPointF> newControlPoints(curve.getControlPoints().size() + degree);
    std::vector<double> newNodalVector(curve.getNodalVector().size() + newNodes.size());

    redefineControlPointsNodalVectorCurve(degree, curve.getNodalVector(), curve.getControlPoints(), newNodes, newNodalVector, newControlPoints);
    return redefineCurve(newControlPoints, newNodalVector, degree, static_cast<int>(curve.getCurvePoints().size()));
}

void AlgBasedCurveConjugation::redefineControlPointsNodalVectorCurve(int newDegreeCurve, const std::vector<double>& oldNodalVector, const std::vector<QPointF> &oldControlPoints, std::vector<double> newNodes,
                                                      std::vector<double>& newNodalVector, std::vector<QPointF> &newControlPoints) const
{
    int numOldControlPoints = static_cast<int>(oldControlPoints.size());    // Коли-во контрольных точек до вставки
    int maxIndexNewNodes = static_cast<int>(newNodes.size()) - 1;

    double a = CalcCurve::findSpanForParameter(newNodes[0], oldNodalVector, newDegreeCurve);
    double b = CalcCurve::findSpanForParameter(newNodes[maxIndexNewNodes], oldNodalVector, newDegreeCurve) + 1;

    for (int i = 0; i < a - newDegreeCurve + 1; ++i)
        newControlPoints[i] = oldControlPoints[i];

    for (int i = b - 1; i < numOldControlPoints; ++i)
        newControlPoints[i + maxIndexNewNodes + 1] = oldControlPoints[i];

    for (int i = 0; i < a + 1; ++i)
        newNodalVector[i] = oldNodalVector[i];

    for (int i = b + newDegreeCurve; i < numOldControlPoints + newDegreeCurve + 1; ++i)
        newNodalVector[i + maxIndexNewNodes + 1] = oldNodalVector[i];

    int ind = b + newDegreeCurve - 1;
    int k = b + newDegreeCurve + maxIndexNewNodes;

    for (int i = maxIndexNewNodes; i > -1; --i)
    {
        while ((newNodes[i] <= oldNodalVector[ind]) && (ind > a))
        {
            newControlPoints[k - newDegreeCurve - 1] = oldControlPoints[ind - newDegreeCurve - 1];
            newNodalVector[k] = oldNodalVector[ind];
            --k, --ind;
        }

        newControlPoints[k - newDegreeCurve - 1] = newControlPoints[k - newDegreeCurve];

        for (int j = 1; j < newDegreeCurve + 1; ++j)
        {
            int temp = k - newDegreeCurve + j;
            double alpha = newNodalVector[k + j] - newNodes[i];

            if (alpha == 0)
                newControlPoints[temp - 1] = newControlPoints[temp];
            else
            {
                alpha /= (newNodalVector[k + j] - oldNodalVector[ind - newDegreeCurve + j]);
                newControlPoints[temp - 1].setX(alpha * newControlPoints[temp - 1].x() + (1 - alpha) * newControlPoints[temp].x());
                newControlPoints[temp - 1].setY(alpha * newControlPoints[temp - 1].y() + (1 - alpha) * newControlPoints[temp].y());
            }
        }

        newNodalVector[k] = newNodes[i];
        --k;
    }
}
