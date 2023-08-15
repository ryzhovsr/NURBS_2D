#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Graph2D.h"
#include "Metrics.h"
#include "NativeApproxAlg.h"
#include "ApproxAlgBasedIntegralNorm.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    const std::vector<QPointF> CONTROL_POINTS   // Контрольные точки определяющего многоугольника
    {
        {3, 5},
        {6, 7},
        {9, 3},
        {12, 6},
        {15, 5}
    };

    const std::vector<double> WEIGHTS(CONTROL_POINTS.size(), 1);   // Весовые коэффициенты контрольных точек
    const int CURVE_NUM_POINTS = 100;   // Кол-во точек, из которых будет состоять кривая
    const int DEGREE = 4;   // Степень кривой

    Curve curve1(CONTROL_POINTS, WEIGHTS, DEGREE, CURVE_NUM_POINTS);

    Graph2D canvas(ui->canvas);
    canvas.drawCurve(curve1, "Кривая 1", QColor(20, 150, 30));
    canvas.drawDefiningPolygon(curve1.getControlPoints(), "Определяющий многоугольник");

    ApproxAlgBasedIntegralNorm a;
    Curve curve2 = a.approximateCurve(curve1, DEGREE - 1);

    canvas.drawCurve(curve2, "Кривая 2", QColor(202, 150, 230));
    canvas.drawDefiningPolygon(curve2.getControlPoints(), "", QColor(0, 0, 0), Qt::DashLine);

    const double DISTANSE_1 = Metrics::calcHausdorffMetric(curve1, curve2);
    qDebug() << DISTANSE_1;
}

/*
// Пример построения с крылом 1
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    int approximateDegree = 22;

    const std::vector<QPointF> CONTROL_POINTS    // Крыло
    {
        {0, 0},
        {0.005, 0.02304},
        {0.01, 0.03348},
        {0.02, 0.04839},
        {0.04, 0.07084},
        {0.06, 0.0873463},
        {0.08, 0.1000003},
        {0.1, 0.1103550},
        {0.15, 0.1284326},
        {0.2, 0.1380163},
        {0.25, 0.1414286},
        {0.3, 0.1412410},
        {0.35, 0.1383454},
        {0.4, 0.1333390},
        {0.45, 0.126360},
        {0.5, 0.117954},
        {0.55, 0.1083934},
        {0.6, 0.0981257},
        {0.65, 0.0872923},
        {0.7, 0.075983},
        {0.75, 0.064175},
        {0.8, 0.0520277},
        {0.85, 0.0393737},
        {0.9, 0.0265526},
        {0.95, 0.013258},
        {1, 0}
    };

    const std::vector<double> WEIGHTS(CONTROL_POINTS.size(), 1);   // Весовые коэффициенты контрольных точек
    const int CURVE_NUM_POINTS = 100;   // Кол-во точек, из которых будет состоять кривая
    const int DEGREE = 24;   // Степень кривой

    Curve originalCurve(CONTROL_POINTS, WEIGHTS, DEGREE, CURVE_NUM_POINTS);
    originalCurve.calcCurve();
    const double ORIGINAL_CURVE_CURVATURE = Metrics::calcCurveCurvature(originalCurve.getCurvePoints());

    qDebug() << "Исходная кривая:\n"
                "Степень =" << DEGREE <<
                "\nКривизна =" << ORIGINAL_CURVE_CURVATURE << '\n';

    Graph2D canvas(ui->canvas);
    canvas.drawCurve(originalCurve, "Исходная кривая (24 степень)", QColor(30, 144, 255), Qt::PenStyle::SolidLine, 4);
    canvas.drawDefiningPolygon(originalCurve.getControlPoints(), "Определяющий многоугольник");

    //////Первый способ - разбили на безье, понизили, соединили, добавили узлы, и по новой

    Curve newCurve_1(CONTROL_POINTS, WEIGHTS, approximateDegree, CURVE_NUM_POINTS);
    newCurve_1.calcCurve();

    const double DISTANSE_1 = Metrics::calcHausdorffMetric(originalCurve, newCurve_1);
    const double CURVATURE_1 = Metrics::calcCurveCurvature(newCurve_1.getCurvePoints());
    double diff1 = Metrics::calcQuadraticDifference(originalCurve, newCurve_1);

    qDebug() << "Сложный алгоритм:\n"
                "Новая степень =" << newCurve_1.getDegree() <<
                "\nРасстояние между исходной кривой =" << DISTANSE_1 <<
                "\nНовая кривизна =" << CURVATURE_1 <<
                "\nНовая квадр. разность =" << diff1 << '\n';

    canvas.drawCurve(newCurve_1, "Новая кривая, аппр. интегральным способом", QColor(100, 0, 193), Qt::PenStyle::DashLine);
}
*/
MainWindow::~MainWindow()
{
    delete ui;
}

