#pragma once

#include "qcustomplot.h"
#include "Curve.h"

class Graph2D
{
public:
    Graph2D(QCustomPlot *canvas, const QString &title = "") noexcept;

    // Рисует касательную (первую производную в точке кривой (pointNURBS))
    void drawTangent(const CurvePoint& pointNURBS) const noexcept;

    // Рисует определяющий многоугольник с вершинами
    void drawPolygon(const std::vector<QPointF>& polygonPoints, const QString& name, const Qt::PenStyle& penStyle = Qt::SolidLine, const QColor& color = QColor(0, 0, 0), double width = 1) const noexcept;

    // Рисует точку
    void drawPoint(QPointF point, double width = 5, const QString& label = "", const QColor& color = QColor(0, 0, 0)) const noexcept;

    // Рисуют линию по двум точкам
    void drawLine(QPointF point1, QPointF point2, const QColor& color = QColor(0, 0, 0), double width = 3.5) const noexcept;
    void drawLine(double x1, double y1, double x2, double y2, const QColor& color = QColor(0, 0, 0), double width = 3.5) const noexcept;

    // Рисует двойную стрелку "<-->" по двум точкам
    void drawDoubleArrow(QPointF point1, QPointF point2) const noexcept;

    // Рисует стрелку "-->" по двум точкам
    void drawArrow(QPointF point1, QPointF point2) const noexcept;

    // Рисует линию (касательную) c центром в точке кривой (point)
    void drawTangentCentred(const CurvePoint& curvePoint, const QColor& color = QColor(0, 100, 0), double width = 2.8) const noexcept;

    // Рисует кривую NURBS
    void drawCurve(const Curve& pointsNURBS, const QString& name, const Qt::PenStyle& penStyle = Qt::PenStyle::SolidLine, const QColor& color = QColor(0, 0, 0, 255), double width = 3) const noexcept;
    void drawCurve(const std::vector<CurvePoint>& pointsNURBS, const QString& name, const Qt::PenStyle& penStyle = Qt::PenStyle::SolidLine, const QColor& color = QColor(0, 0, 0, 255), double width = 3) const noexcept;

    // Рисует надпись
    void drawLable(QPointF point, const QString& label, double fontSize = 10) const noexcept;

    // Рисует надпись со стрелкой
    void drawLableWithArrow(QPointF point1, QPointF point2, const QString& label) const noexcept;

    // Рисует вектор первой производной кривой
    void drawFirstDeriv(const Curve& pointsNURBS, const QString& name, const Qt::PenStyle& penStyle = Qt::PenStyle::SolidLine, const QColor& color = QColor(0, 0, 0, 255), double width = 1.5) const noexcept;

    // Рисует вектор второй производной кривой
    void drawSecondDeriv(const Curve& pointsNURBS, const QString& name, const Qt::PenStyle& penStyle = Qt::PenStyle::SolidLine, const QColor& color = QColor(0, 0, 0, 255), double width = 1.5) const noexcept;

    // Рисует две кривые
    void drawCurves(const std::pair<std::vector<CurvePoint>, std::vector<CurvePoint>>& twoCurves) const noexcept;

    // Рисует перпендикуляры между кривыми
    void drawNormalsBetweenCurves(const Curve& curve_1, const Curve& curve_2, const QColor& color = QColor(0, 0, 0)) const noexcept;

private:
    QCustomPlot* _canvas; // Указатель на окно для рисования
    QString _title;
};
