# NURBS_2D

Данный проект является моей дипломной работой по созданию, построению и аппроксимации NURBS (Non-uniform rational B-spline). NURBS - это математическая модель, использующая базисные сплайны (B-сплайны), которая используется в компьютерной графике для представления кривых и поверхностей.

Проект представляет собой приложение, созданное в QtCreator на C++. Для построения кривых используется open source библиотека QCustomPlot.

### Алгоритмы аппроксимации
1. **Нативный алгоритм** - понижение степени на осное исходного определяющего многоугольника;
2. **Алгоритм на основе интегральной нормы**, минимизирующий кривизну и квадратичное отклонение;
3. **Алгоритм на основе сопражения кривых**, который состоит из следующей последовательности математических процедур - представление исходного сплайна набором кривых Безье, понижение степени каждой кривой Безье и преобразование каждой кривой для полностью гладкого сопряжения.

### Метрики разности двух кривых
1. **Метрика Хаусдорфа** - наибольший перпендикуляр между двумя кривыми;
2. **Изменение кривизны сплайна** - разность интегралов вторых производных;
3. **Квадратичная разность между кривыми**.

С помощью данных метрик можно сравнивать погрешность аппроксимации всех трёх алгоритмов.