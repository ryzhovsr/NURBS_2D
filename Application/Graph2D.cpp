#include "Graph2D.h"

Graph2D::Graph2D(QCustomPlot *canvas, const QString& title) noexcept
    : _canvas { canvas }, _title { title }
{
    _canvas->clearGraphs(); // Очищаем все графики, если они были
    _canvas->legend->setVisible(true); // Включаем легенду графика
    _canvas->setInteractions(QCP :: iRangeDrag | QCP :: iRangeZoom); // Делаем график перетаскиваемым + масштабирование колеса прокрутки
    _canvas->plotLayout()->insertRow(0); // Вставляем строку для создания оглавления canvas
    _canvas->plotLayout()->addElement (0, 0, new QCPTextElement(_canvas, _title, QFont("sans", 12))); // Добавляем оглавление
    _canvas->xAxis->setRange(-10, 10);
    _canvas->yAxis->setRange(-10, 10);
}

/*
void Graph2D::drawTangent(const CurvePoint& pointNURBS) const noexcept
{
    QCPItemLine *line = new QCPItemLine(_canvas);
    line->setHead(QCPLineEnding::esFlatArrow);
    line->start->setCoords(pointNURBS.point.x(), pointNURBS.point.y());
    line->end->setCoords(pointNURBS.firstDeriv.x() + pointNURBS.point.x(), pointNURBS.firstDeriv.y() + pointNURBS.point.y());
    _canvas->replot();
}

void Graph2D::drawPolygon(const std::vector<QPointF>& polygonPoints, const QString& name, const Qt::PenStyle& penStyle, const QColor& color, double width) const noexcept
{
    QCPCurve *shape = new QCPCurve(_canvas->xAxis, _canvas->yAxis);
    shape->setPen(color);
    shape->setLineStyle(QCPCurve::lsNone); // Убираем линии
    shape->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 6)); // Формируем вид точек
    QPen pen;
    pen.setStyle(penStyle);
    pen.setWidth(width);
    shape->setPen(pen);

    // uint counter = 0; // Для лейблов каждой точки многоугольника

    for (const auto& point: polygonPoints) // Рисуем точки
    {
        shape->addData(point.x(), point.y());

        // Делает подписи к каждой вершине многоугольника
        /*
        QCPItemText *label = new QCPItemText(canvas);
        label->position->setCoords(point[0] + 0.35, point[1] - 0.2);
        label->setFont(QFont("sans", 10));
        label->setText(QString("P%1").arg(counter++));
        */
/*    }

    shape->setLineStyle(QCPCurve::lsLine); // Добавляем линии
    shape->setName(name); // Устанавливаем название полигона в легенде графика
    _canvas->replot();
}

void Graph2D::drawPoint(QPointF point, double width, const QString& label, const QColor& color) const noexcept
{
    _canvas->addGraph();
    _canvas->graph()->setPen(color);
    _canvas->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, width)); // Формируем вид точек
    _canvas->graph()->setLineStyle(QCPGraph::lsNone);
    _canvas->graph()->addData(point.x, point.y);
    _canvas->legend->removeItem(_canvas->legend->itemCount() - 1); // Удаляем точку из легенды, если надо

    if (!label.isEmpty()) // Если текст label не пустой
    {
        QCPItemText *labelPoint = new QCPItemText(_canvas); // Делаем подпись к точке
        labelPoint->position->setCoords(point.x + 0.2, point.y - 0.2);
        labelPoint->setText(label);
    }

    _canvas->replot();
}

void Graph2D::drawLine(QPointF point1, QPointF point2, const QColor& color, double width) const noexcept
{
    QCPItemLine *line = new QCPItemLine(_canvas);
    QPen pen;
    pen.setStyle(Qt::PenStyle::DashLine);
    pen.setColor(color);
    pen.setWidth(width);
    line->setPen(pen);
    line->start->setCoords(point1.x(), point1.y());
    line->end->setCoords(point2.x(), point2.y());
    _canvas->replot();
}

void Graph2D::drawLine(double x1, double y1, double x2, double y2, const QColor& color, double width) const noexcept
{
    drawLine(QPointF(x1, y1), QPointF(x2, y2), color, width);
}

void Graph2D::drawDoubleArrow(QPointF point1, QPointF point2) const noexcept
{
    QCPItemLine *line = new QCPItemLine(_canvas);
    line->setHead(QCPLineEnding::esFlatArrow);
    line->setTail(QCPLineEnding::esFlatArrow);
    line->start->setCoords(point1.x, point1.y);
    line->end->setCoords(point2.x, point2.y);
    _canvas->replot();
}

void Graph2D::drawArrow(QPointF point1, QPointF point2) const noexcept
{
    QCPItemLine *line = new QCPItemLine(_canvas);
    line->setHead(QCPLineEnding::esFlatArrow);
    line->start->setCoords(point1.x, point1.y);
    line->end->setCoords(point2.x, point2.y);
    _canvas->replot();
}

void Graph2D::drawTangentCentred(const CurvePoint& curvePoint, const QColor& color, double width) const noexcept
{
    QCPItemLine *line = new QCPItemLine(_canvas);
    QPen pen;
    pen.setColor(color);
    pen.setWidth(width);
    line->setPen(pen);
    line->start->setCoords(curvePoint.point.x - curvePoint.firstDeriv.x / 10, curvePoint.point.y - curvePoint.firstDeriv.y / 10);
    line->end->setCoords(curvePoint.point.x + curvePoint.firstDeriv.x / 10, curvePoint.point.y + curvePoint.firstDeriv.y / 10);
    _canvas->replot();
}
*/
void Graph2D::drawCurve(const Curve& pointsNURBS, const QString& name, const Qt::PenStyle& penStyle, const QColor& color, double width) const noexcept
{
    QCPCurve *curve = new QCPCurve(_canvas->xAxis, _canvas->yAxis);
    QPen pen;
    pen.setColor(QColor(color));
    pen.setStyle(penStyle);
    pen.setWidthF(width);
    curve->setPen(pen);

    for (const auto& pointCurve: pointsNURBS.getCurvePoints())
        curve->addData(pointCurve.point.x(), pointCurve.point.y());

    curve->setName(name);
    _canvas->replot();
}
/*
void Graph2D::drawCurve(const std::vector<CurvePoint>& pointsNURBS, const QString& name, const Qt::PenStyle& penStyle, const QColor& color, double width) const noexcept
{
    QCPCurve *curve = new QCPCurve(_canvas->xAxis, _canvas->yAxis);
    QPen pen;
    pen.setColor(QColor(color));
    pen.setStyle(penStyle);
    pen.setWidthF(width);
    curve->setPen(pen);

    for (const auto& pointCurve: pointsNURBS)
        curve->addData(pointCurve.point.x, pointCurve.point.y);

    curve->setName(name);
    _canvas->replot();
}

void Graph2D::drawLable(QPointF point, const QString& label, double fontSize) const noexcept
{
    QCPItemText *labelText = new QCPItemText(_canvas);
    labelText->setFont(QFont("sans", fontSize));
    labelText->position->setCoords(point.x, point.y);
    labelText->setText(label);
    _canvas->replot();
}

void Graph2D::drawLableWithArrow(QPointF point1, QPointF point2, const QString& label) const noexcept
{
    QCPItemText *labelText = new QCPItemText(_canvas);
    labelText->setFont(QFont("sans", 10));
    labelText->position->setCoords(point1.x + 0.3, point1.y + 0.7);
    labelText->setText(label);

    QCPItemLine *line = new QCPItemLine(_canvas);
    line->setHead(QCPLineEnding::esFlatArrow);
    line->start->setCoords(point1.x, point1.y);
    line->end->setCoords(point2.x, point2.y);
    _canvas->replot();
}

void Chart::drawFirstDeriv(const Curve& pointsNURBS, const QString& name, const Qt::PenStyle& penStyle, const QColor& color, double width) const noexcept
{
    QCPCurve *curve = new QCPCurve(_canvas->xAxis, _canvas->yAxis);
    QPen pen;
    pen.setColor(QColor(color));
    pen.setStyle(penStyle);
    pen.setWidthF(width);
    curve->setPen(pen);

    for (const auto& pointCurve: pointsNURBS.getPointsNURBS())
        curve->addData(pointCurve.firstDeriv.x + pointCurve.point.x, pointCurve.firstDeriv.y + pointCurve.point.y);

    curve->setName(name);
    _canvas->replot();
}

void Graph2D::drawSecondDeriv(const Curve& pointsNURBS, const QString& name, const Qt::PenStyle& penStyle, const QColor& color, double width) const noexcept
{
    QCPCurve *curve = new QCPCurve(_canvas->xAxis, _canvas->yAxis);
    QPen pen;
    pen.setColor(QColor(color));
    pen.setStyle(penStyle);
    pen.setWidthF(width);
    curve->setPen(pen);

    for (const auto& pointCurve: pointsNURBS.getPointsNURBS())
        curve->addData(pointCurve.secondDeriv.x + pointCurve.point.x, pointCurve.secondDeriv.y + pointCurve.point.y);

    curve->setName(name);
    _canvas->replot();
}

void Graph2D::drawCurves(const std::pair<std::vector<CurvePoint>, std::vector<CurvePoint>>& twoCurves) const noexcept
{
    drawCurve(twoCurves.first, "Допустимое отклонение", Qt::PenStyle::DashLine);
    drawCurve(twoCurves.second, "", Qt::PenStyle::DashLine);
}

void Graph2D::drawNormalsBetweenCurves(const Curve& curve1, const NURBS& curve2, const QColor& color) const noexcept
{
    for (size_t i = 0; i < curve2.getPointsNURBS().size(); i += 2)
    {
        CurvePoint nearestPoint = curve1.findPointNURBS(curve2.getPointsNURBS()[i].point);
        // Рисуем прямые между точками двумя кривымии (перпендикуляры)
        drawLine(Point(curve2.getPointsNURBS()[i].point.x, curve2.getPointsNURBS()[i].point.y), Point(nearestPoint.point.x, nearestPoint.point.y), color);
    }
}
*/
