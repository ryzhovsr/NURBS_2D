QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ApproxAlgBasedIntegralNorm.cpp \
    ApproxAlgs.cpp \
    CalcCurve.cpp \
    Curve.cpp \
    EigenMatrixOperations.cpp \
    Graph2D.cpp \
    IMatrixOperations.cpp \
    MathUtils.cpp \
    Metrics.cpp \
    NativeApproxAlg.cpp \
    main.cpp \
    mainwindow.cpp \
    qcustomplot.cpp

HEADERS += \
    ApproxAlgBasedIntegralNorm.h \
    ApproxAlgs.h \
    CalcCurve.h \
    Curve.h \
    EigenMatrixOperations.h \
    Graph2D.h \
    CurvePoint.h \
    IMatrixOperations.h \
    MathUtils.h \
    Metrics.h \
    NativeApproxAlg.h \
    mainwindow.h \
    qcustomplot.h

FORMS += \
    mainwindow.ui

INCLUDEPATH += C:\Main\Projects\NURBS_2D\Dependencies\include\eigen-3.4.0

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES +=
