#include <windows.h> 
#include <conio.h>
#include <cmath>
#include <iostream>
#include <iomanip> 

/* В ПЕРВЫЙ РАЗ ДОЛГО СОБИРАЕТСЯ
Чтобы графический интерфейс заработал, надо зайти в:
Отладка ->
*Название проета*: свойства... ->
Cвойства конфигурации ->
Дополнительно ->
Набор символов ->
тут меняем юникод на многобайтовую кодировку */

int n = 10; //количество узлов интерполяции
double left = -3, right = 3; //граници интервала
double h = (right - left) / (n - 1); //длина отдного отрезка интерполирования

//выделяем место под коэфициенты для сплайнов на n-1 отрезках (с запасом для рассчетов)
double* a = new double[n + 1];
double* b = new double[n + 1];
double* c = new double[n + 1];
double* d = new double[n + 1];

void point(HDC hdc, double X, double Y); //ставит точку
void cross(HDC hdc, double X, double Y); // ставит крестик
void axises(HDC hdc, int X0, int Y0, int Width, int Height); //строит оси
void countСoeffs(); // считает все коэффициенты
void outСoeffs(); //выводим коэффициенты
void Rmax(); //максимальная ошибка
void progon(double** arr, double* c, double* y, int m); //прогонка с прошлого семака
void drawGraph(double f(double), double left, double right, bool crosses /*отмечать ли узлы интерпояции*/); //штука строит график функции на указнном отрезке
double spline(double x); //собсвтенно, сплайн

// графический интерфейс тут имеет начало координат в левом верхнем углу консоли
// а также инвертированную ось y, так что перед функциями ставим -
double func(double x) {
    return -(8 * sin(4 * x) + pow(x, 2) / 2 - 5);
    //return -sin(sin(x))/cos(sin(5*x));
    //return 10*pow(10*x, 2);
    //return pow(x, 2);
}

void main() {

    countСoeffs(); //посчитаем коэффициенты сплайна для каждого отрезка
    outСoeffs(); //выводим коэффициенты
    Rmax(); //выводим максимальную ошибку
    drawGraph(func, left, right, 1); //строим график функции func на указнном отрезке
    drawGraph(spline, left, right, 0); // строим сплайн    

    _getch(); //ждем пока считается символ
}

void countСoeffs() { // все считаем тупа по википедии

    //ищем все a-коэфф.
    for (int it = 0; it < n + 1; it++) a[it] = func(left + h * it);

    //ищем с-коэфф., решив систему методом прогонки
    double* tmpC = new double[n]; //решение системы, которое потом надо будет сдвинуть по индексам
    double* tmpY = new double[n];
    for (int i = 0;i < n;i++) { //считаем правую часть
        tmpY[i] = (3.0 / pow(h, 2)) * (a[i] - 2 * a[i + 1] + a[i + 2]);
    }
    double** arr = new double* [n]; //задаем трехдиагональную матрицу коэффициентов перед h (на вики это как если бы все h итые были равны)
    for (int i = 0;i < n;i++) {
        arr[i] = new double[n];
        for (int j = 0;j < n;j++) {
            if (i == j) {
                arr[i][j] = 4;
                continue;
            }
            if (abs(i - j) == 1) {
                arr[i][j] = 1;
                continue;
            }
            arr[i][j] = 0;
        }
    }
    progon(arr, tmpC, tmpY, n - 1); //метод прогонки. он запишет ответ в tmpC

    c[0] = 0; //считаем окончательные значения c-коэфф.
    for (int it = 1; it < n; it++) c[it] = tmpC[it - 1];
    c[n] = 0;

    //ищем все b-коэфф.
    for (int it = 1; it < n; it++) b[it] = (a[it] - a[it - 1]) / h + (2 * c[it] + c[it - 1]) * (h / 3);

    //ищем все d-коэфф.
    for (int it = 1; it < n; it++) d[it] = (c[it] - c[it - 1]) / (3 * h);
}

void outСoeffs() {
    std::cout << "\n\ta[i]\tb[i]\tc[i]\td[i]\t\n";
    for (int i = 1;i < n;i++) {
        std::cout << std::setprecision(3) << "     " << i << "\t" << a[i] << "\t" << b[i] << "\t" << c[i] << "\t" << d[i] << "\n";
    }
    //std::cout << ;

}

void Rmax() { //ищем максимальную ошибку интерполяции
    double xmax, max = 0, fmax = 0, fmin = 0;
    for (double x = left; x <= right; x += 0.01) {
        if (abs(spline(x) - func(x)) > max) {
            max = abs(spline(x) - func(x));
            xmax = x;
        }
        if (fmax < func(x)) fmax = func(x);
        if (fmin > func(x)) fmin = func(x);
    }
    //посчитали тут еще отсноистельную ошибку, как отношение максимально ошибки к длине диапазона значений функции
    std::cout << "\n     maximum error = " << max << "\n     relative error = " << 100 * max / (fmax - fmin) << "%";
}

double spline(double x) {
    double tmp = (x - left) / h;
    int index = (int)tmp + 1; //номер отрезка, откуда взят x
    double x_l = (left + h * index); //правый конец отрезка интерполяции

    return d[index] * pow((x - x_l), 3) + c[index] * pow((x - x_l), 2) + b[index] * (x - x_l) + a[index];
}

void point(HDC hdc, double X, double Y) // костыльно ставит точку по указанным координатам
{
    MoveToEx(hdc, X, Y, NULL);
    LineTo(hdc, X, Y);
}

void axises(HDC hdc, int X0, int Y0, int Width, int Height) { //строим оси координа в X0 Y0 (они далее)

    //я не знаю, как менять характеристики пера, так что костыльно пересоздаю его тут и далее
    HPEN p1, p2 = CreatePen(PS_DASH, 0, RGB(0, 0, 0)); // создаем перо
    p1 = (HPEN)SelectObject(hdc, p2);  // заносим перо в контекст рисования

    MoveToEx(hdc, X0, 0, NULL);
    LineTo(hdc, X0, Height);
    MoveToEx(hdc, 0, Y0, NULL);
    LineTo(hdc, Width, Y0);

    SelectObject(hdc, p1);   // возвращаем старое перо
    DeleteObject(p2);    // удаляем созданное перо
}

void cross(HDC hdc, double X, double Y) { //ставит красный крестик в узле интерполяции 

    HPEN p1, p2 = CreatePen(PS_SOLID, 1, RGB(255, 0, 0)); // создаем перо
    p1 = (HPEN)SelectObject(hdc, p2);  // заносим перо в контекст рисования

    //ставим крестик
    MoveToEx(hdc, X + 7, Y + 7, NULL);
    LineTo(hdc, X - 7, Y - 7);
    MoveToEx(hdc, X - 7, Y + 7, NULL);
    LineTo(hdc, X + 7, Y - 7);

    SelectObject(hdc, p1);   // возвращаем старое перо
    DeleteObject(p2);    // удаляем созданное перо
}

void drawGraph(double f(double), double left, double right, bool crosses)
{
    HWND hwnd;
    char Title[1024];
    GetConsoleTitle(Title, 1024); // узнаем имя окна
    hwnd = FindWindow(NULL, Title); // узнаем hwnd окна
    RECT rc;
    GetClientRect(hwnd, &rc);
    int Width = rc.right; //ширина окна
    int Height = rc.bottom; //высота окна
    HDC hdc = GetDC(hwnd);  // получаем контекст для рисования

    int R = 0, G = 0, B = 0;
    if (crosses) B = 255; //чтобы графики с узлами и без были разного цвета
    else G = 255;
    HPEN p1, p2 = CreatePen(PS_SOLID, 2, RGB(R, G, B)); // создаем перо
    p1 = (HPEN)SelectObject(hdc, p2);  // заносим перо в контекст рисования

    double scaleX = 80, scaleY = 15; //масштабы растягивания по осям
    double x, X, Y;
    int X0 = Width * 0.7; // начало координат
    int Y0 = Height * 0.5; // начало координат (помним, что y инвертирована)
    axises(hdc, X0, Y0, Width, Height); //рисуем оси
    /*Если что-то не вошло, можно двигать не только масштаб и центр СК,
    но можно и просто консоль растянуть в нужную сторну, закрыть ее, и перезапустить программу*/


    for (x = left; x <= right; x += 0.001)
    {
        X = scaleX * x + X0; //исковая координата, куда поставится точка
        Y = scaleY * f(x) + Y0; //такая же игрековая
        point(hdc, X, Y); //ставим точку (из таких состоит график)
    }

    if (crosses) //если мы попросили строить узлы интерполяции, то сторим их
    {
        for (int i = 0; i < n; i++) cross(hdc, scaleX * (left + h * i) + X0, scaleY * f(left + h * i) + Y0);
    }

    SelectObject(hdc, p1);   // возвращаем старое перо
    ReleaseDC(hwnd, hdc);    // освобождаем контекст рисования
    DeleteObject(p2);       // удаляем созданное перо
}

void progon(double** arr, double* C, double* y, int m) {
    double* y_koef = new double[m];
    double* alpha_koef = new double[m - 1];
    double* beta_koef = new double[m - 1];

    double* a = new double[m];
    double* b = new double[m];
    double* _c = new double[m];

    for (int i = 0; i < m; i++)
    {
        if (i != 0) a[i] = arr[i][i - 1];
        b[i] = arr[i][i];
        if (i != n - 1) _c[i] = arr[i][i + 1];
    }

    //для первой строки
    y_koef[0] = b[0];
    alpha_koef[0] = -_c[0] / y_koef[0];
    beta_koef[0] = y[0] / y_koef[0];

    //для строк 2 - (n-1)
    for (int i = 1; i < m; i++)
    {
        y_koef[i] = b[i] + a[i] * alpha_koef[i - 1];
        if (i < n - 1) alpha_koef[i] = -_c[i] / y_koef[i];
        beta_koef[i] = (y[i] - a[i] * beta_koef[i - 1]) / y_koef[i];
    }
    C[n - 1] = beta_koef[m - 1];
    for (int i = n - 2; i > -1; i--)
    {
        C[i] = alpha_koef[i] * C[i + 1] + beta_koef[i];
    }
}