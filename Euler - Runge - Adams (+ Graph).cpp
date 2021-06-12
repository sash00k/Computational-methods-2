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


int n =10; //количество узлов интерполяции
double x_0 = 0;
double y_0 = 0;
double left = x_0, right = 2; //граници интервала
double h = (right - left) / (n - 1); //длина отдного отрезка интерполирования

double* xs = new double[n];
double* ys = new double[n];

double func(double x, double y) {
    //return pow(y,2/3);//3
    //return 2 * pow(abs(y), 0.5);//2
    //return pow(y, 2);//1
    return 3 * x * x * y + x * x * exp(pow(x, 3));
}

double sol(double x) {
    //return (double)pow(x, 3.0) / 27.0;//3
    //return (x >= 0) ? x * x : (-1) * x * x; //2
    //return 1.0 / (1 - x);//1
    return exp(pow(x, 3)) * pow(x, 3) / 3;
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

void cross(HDC hdc, double X, double Y, char color) { //ставит красный крестик в узле интерполяции 
    HPEN p1, p2;
    switch (color)
    {
    case 'r':
        p2 = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
        break;
    case 'g':
        p2 = CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
        break;
    case 'b':
        p2 = CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
        break;
    case 'y':
        p2 = CreatePen(PS_SOLID, 1, RGB(0, 255, 255));
        break;
    default:
        p2 = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
        break;
    }

    p1 = (HPEN)SelectObject(hdc, p2);  // заносим перо в контекст рисования

    //ставим крестик
    MoveToEx(hdc, X + 7, Y + 7, NULL);
    LineTo(hdc, X - 7, Y - 7);
    MoveToEx(hdc, X - 7, Y + 7, NULL);
    LineTo(hdc, X + 7, Y - 7);

    SelectObject(hdc, p1);   // возвращаем старое перо
    DeleteObject(p2);    // удаляем созданное перо
}

//3
//double scaleX = 50, scaleY = 50;
//double coefX = 0.5, coefY = 0.8;

//2
double scaleX = 200, scaleY = 50;
double coefX = 0.5, coefY = 0.8;

void drawGraph(double left, double right, bool crosses, char color)
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

    double x, X, Y;
    int X0 = Width * coefX; // начало координат
    int Y0 = Height * coefY; // начало координат (помним, что y инвертирована)
    axises(hdc, X0, Y0, Width, Height); //рисуем оси
    /*Если что-то не вошло, можно двигать не только масштаб и центр СК,
    но можно и просто консоль растянуть в нужную сторну, закрыть ее, и перезапустить программу*/

    //for (x = left; x <= right; x += 0.001)
    //{
    //    X = scaleX * x + X0; //исковая координата, куда поставится точка
    //    Y = scaleY * f(x) + Y0; //такая же игрековая
    //    point(hdc, X, Y); //ставим точку (из таких состоит график)
    //}

    if (crosses) //если мы попросили строить узлы интерполяции, то сторим их 
    {
        for (int i = 0; i < n; i++) cross(hdc, scaleX * (xs[i]) + X0, -scaleY * ys[i] + Y0, color);
    }

    SelectObject(hdc, p1);   // возвращаем старое перо
    ReleaseDC(hwnd, hdc);    // освобождаем контекст рисования
    DeleteObject(p2);       // удаляем созданное перо
}

void drawFunc(double f(double), double left, double right, bool crosses)
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

    double x, X, Y;
    int X0 = Width * coefX; // начало координат
    int Y0 = Height * coefY; // начало координат (помним, что y инвертирована)
    //axises(hdc, X0, Y0, Width, Height); //рисуем оси
    /*Если что-то не вошло, можно двигать не только масштаб и центр СК,
    но можно и просто консоль растянуть в нужную сторну, закрыть ее, и перезапустить программу*/

    for (x = left; x <= right; x += 0.001)
    {
        X = scaleX * x + X0; //исковая координата, куда поставится точка
        Y = -scaleY * f(x) + Y0; //такая же игрековая
        point(hdc, X, Y); //ставим точку (из таких состоит график)
    }

    if (crosses) //если мы попросили строить узлы интерполяции, то сторим их
    {
        for (int i = 0; i < n; i++) cross(hdc, scaleX * (left + h * i) + X0, -scaleY * f(left + h * i) + Y0, 'y');
    }

    SelectObject(hdc, p1);   // возвращаем старое перо
    ReleaseDC(hwnd, hdc);    // освобождаем контекст рисования
    DeleteObject(p2);       // удаляем созданное перо
}

void euler() {

    xs[0] = x_0;
    ys[0] = y_0;

    for (int i = 1;i < n;i++) {
        ys[i] = ys[i - 1] + h * func(xs[i - 1], ys[i - 1]);
        xs[i] = h + xs[i - 1];
    }

    drawGraph(left, right, 1, 'b');
    drawFunc(sol, left, right, 0);

    //std::cout << "\n\ty[i]\tb[i]\tc[i]\td[i]\t\n";
    std::cout << "euler\n";
    for (int i = 0;i < n;i++) {
        if (ys[i] != 0)
            std::cout << std::setprecision(8) << "     " << i << ":\t" << ys[i] << "\t" << 100 * abs(sol(left + h * i) - ys[i]) / sol(left + h * i) << "\t" << sol(left + h * i) - ys[i] << "\n";
        else
            std::cout << std::setprecision(8) << "     " << i << ":\t" << "0.0000000" << "\t" << "-        " << "\t" << sol(left + h * i) - ys[i] << "\n";
    }
    std::cout << "\n";
    //std::cout << ;

}

void runge() {

    double* ks = new double[4];

    xs[0] = x_0;
    ys[0] = y_0;

    for (int i = 1;i < n;i++) {
        ks[0] = func(xs[i - 1], ys[i - 1]);
        ks[1] = func(xs[i - 1] + h / 2, ys[i - 1] + h * ks[0] / 2);
        ks[2] = func(xs[i - 1] + h / 2, ys[i - 1] + h * ks[1] / 2);
        ks[3] = func(xs[i - 1] + h, ys[i - 1] + h * ks[2]);

        ys[i] = ys[i - 1] + h * (ks[0] + 2 * ks[1] + 2 * ks[2] + ks[3]) / 6;
        xs[i] = h + xs[i - 1];
    }

    drawGraph(left, right, 1, 'y');
    //drawFunc(sol, left, right, 0);

    //std::cout << "\n\ty[i]\tb[i]\tc[i]\td[i]\t\n";
    std::cout << "runge\n";
    for (int i = 0;i < n;i++) {
        if (ys[i] != 0)
            std::cout << std::setprecision(8) << "     " << i << ":\t" << ys[i] << "\t" << 100*abs(sol(left + h * i) - ys[i])/sol(left + h * i) << "\t" << sol(left + h * i) - ys[i] << "\n";
        else
            std::cout << std::setprecision(8) << "     " << i << ":\t" << "0.0000000" << "\t" << "-        " << "\t" << sol(left + h * i) - ys[i] << "\n";
    }
    std::cout << "\n";
    //std::cout << ;

}

void adams() {

    /*{
        xs[0] = x_0;
        ys[0] = y_0;

        xs[1] = xs[0] + h;
        ys[1] = ys[0] + h * func(xs[0], ys[0]);

        xs[2] = xs[1] + h;
        ys[2] = ys[1] + h *
            (
                1.5 * (xs[1], ys[1])
                - 0.5 * func(xs[0], ys[0])
                );

        xs[3] = xs[2] + h;
        ys[3] = ys[2] + h *
            (
            (23.0 / 12) * func(xs[2], ys[2])
                - (16.0 / 12) * func(xs[1], ys[1])
                + (5.0 / 12) * func(xs[0], ys[0])
                );

        xs[4] = xs[3] + h;
        ys[4] = ys[3] + h *
            (
            (55.0 / 24) * func(xs[3], ys[3])
                - (59.0 / 24) * func(xs[2], ys[2])
                + (37.0 / 24) * func(xs[1], ys[1])
                - (9.0 / 24) * func(xs[0], ys[0])
                );
    }*/

    for (int i = 5;i < n;i++) {
        xs[i] = xs[i - 1] + h;
        ys[i] = ys[i - 1] + h * 
          (
          (1901.0 / 720) * func(xs[i - 1], ys[i - 1])
        - (1387.0 / 360) * func(xs[i - 2], ys[i - 2])
        + (109.0 / 30) * func(xs[i - 3], ys[i - 3])
        - (637.0 / 360) * func(xs[i - 4], ys[i - 4])
        + (251.0 / 720) * func(xs[i - 5], ys[i - 5])
          );
    }

    drawGraph(left, right, 1, 'r');
    //drawFunc(sol, left, right, 0);

    //std::cout << "\n\ty[i]\tb[i]\tc[i]\td[i]\t\n";
    std::cout << "adams\n";
    for (int i = 0;i < n;i++) {
        if (ys[i] != 0)
            std::cout << std::setprecision(8) << "     " << i << ":\t" << ys[i] << "\t" << 100 * abs(sol(left + h * i) - ys[i]) / sol(left + h * i) << "\t" << sol(left + h * i) - ys[i] << "\n";
        else
            std::cout << std::setprecision(8) << "     " << i << ":\t" << "0.0000000" << "\t" << "-        " << "\t" << sol(left + h * i) - ys[i] << "\n";
    }
    std::cout << "\n";
    //std::cout << ;

}

void main() {

    euler();
    runge();
    adams();
    _getch();
}