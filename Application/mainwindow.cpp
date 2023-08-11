#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "Curve.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    const std::vector<QPointF> CONTROL_POINTS   // Контрольные точки определяющего многоугольника
    {
        {1.25, 1.3},
        {2.5, 3.9},
        {5.6, 3.9},
        {6.25, 1.3},
        {7.5, 2.6}
    };

    const std::vector<double> WEIGHTS(CONTROL_POINTS.size(), 1);   // Весовые коэффициенты контрольных точек
    const int DEGREE = 2;        // Степень кривой
    const int NUM_POINTS = 100;  // Кол-во точек, из которых будет состоять кривая

    Curve NURBS(CONTROL_POINTS, WEIGHTS, DEGREE, NUM_POINTS);

}

MainWindow::~MainWindow()
{
    delete ui;
}

