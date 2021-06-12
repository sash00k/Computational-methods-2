#include <windows.h> 
#include <conio.h>
#include <cmath>
#include <iostream>
#include <iomanip> 

/* � ������ ��� ����� ����������
����� ����������� ��������� ���������, ���� ����� �:
������� ->
*�������� ������*: ��������... ->
C������� ������������ ->
������������� ->
����� �������� ->
��� ������ ������ �� ������������� ��������� */

int n = 10; //���������� ����� ������������
double left = -3, right = 3; //������� ���������
double h = (right - left) / (n - 1); //����� ������� ������� ����������������

//�������� ����� ��� ����������� ��� �������� �� n-1 �������� (� ������� ��� ���������)
double* a = new double[n + 1];
double* b = new double[n + 1];
double* c = new double[n + 1];
double* d = new double[n + 1];

void point(HDC hdc, double X, double Y); //������ �����
void cross(HDC hdc, double X, double Y); // ������ �������
void axises(HDC hdc, int X0, int Y0, int Width, int Height); //������ ���
void count�oeffs(); // ������� ��� ������������
void out�oeffs(); //������� ������������
void Rmax(); //������������ ������
void progon(double** arr, double* c, double* y, int m); //�������� � �������� ������
void drawGraph(double f(double), double left, double right, bool crosses /*�������� �� ���� �����������*/); //����� ������ ������ ������� �� �������� �������
double spline(double x); //����������, ������

// ����������� ��������� ��� ����� ������ ��������� � ����� ������� ���� �������
// � ����� ��������������� ��� y, ��� ��� ����� ��������� ������ -
double func(double x) {
    return -(8 * sin(4 * x) + pow(x, 2) / 2 - 5);
    //return -sin(sin(x))/cos(sin(5*x));
    //return 10*pow(10*x, 2);
    //return pow(x, 2);
}

void main() {

    count�oeffs(); //��������� ������������ ������� ��� ������� �������
    out�oeffs(); //������� ������������
    Rmax(); //������� ������������ ������
    drawGraph(func, left, right, 1); //������ ������ ������� func �� �������� �������
    drawGraph(spline, left, right, 0); // ������ ������    

    _getch(); //���� ���� ��������� ������
}

void count�oeffs() { // ��� ������� ���� �� ���������

    //���� ��� a-�����.
    for (int it = 0; it < n + 1; it++) a[it] = func(left + h * it);

    //���� �-�����., ����� ������� ������� ��������
    double* tmpC = new double[n]; //������� �������, ������� ����� ���� ����� �������� �� ��������
    double* tmpY = new double[n];
    for (int i = 0;i < n;i++) { //������� ������ �����
        tmpY[i] = (3.0 / pow(h, 2)) * (a[i] - 2 * a[i + 1] + a[i + 2]);
    }
    double** arr = new double* [n]; //������ ���������������� ������� ������������� ����� h (�� ���� ��� ��� ���� �� ��� h ���� ���� �����)
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
    progon(arr, tmpC, tmpY, n - 1); //����� ��������. �� ������� ����� � tmpC

    c[0] = 0; //������� ������������� �������� c-�����.
    for (int it = 1; it < n; it++) c[it] = tmpC[it - 1];
    c[n] = 0;

    //���� ��� b-�����.
    for (int it = 1; it < n; it++) b[it] = (a[it] - a[it - 1]) / h + (2 * c[it] + c[it - 1]) * (h / 3);

    //���� ��� d-�����.
    for (int it = 1; it < n; it++) d[it] = (c[it] - c[it - 1]) / (3 * h);
}

void out�oeffs() {
    std::cout << "\n\ta[i]\tb[i]\tc[i]\td[i]\t\n";
    for (int i = 1;i < n;i++) {
        std::cout << std::setprecision(3) << "     " << i << "\t" << a[i] << "\t" << b[i] << "\t" << c[i] << "\t" << d[i] << "\n";
    }
    //std::cout << ;

}

void Rmax() { //���� ������������ ������ ������������
    double xmax, max = 0, fmax = 0, fmin = 0;
    for (double x = left; x <= right; x += 0.01) {
        if (abs(spline(x) - func(x)) > max) {
            max = abs(spline(x) - func(x));
            xmax = x;
        }
        if (fmax < func(x)) fmax = func(x);
        if (fmin > func(x)) fmin = func(x);
    }
    //��������� ��� ��� �������������� ������, ��� ��������� ����������� ������ � ����� ��������� �������� �������
    std::cout << "\n     maximum error = " << max << "\n     relative error = " << 100 * max / (fmax - fmin) << "%";
}

double spline(double x) {
    double tmp = (x - left) / h;
    int index = (int)tmp + 1; //����� �������, ������ ���� x
    double x_l = (left + h * index); //������ ����� ������� ������������

    return d[index] * pow((x - x_l), 3) + c[index] * pow((x - x_l), 2) + b[index] * (x - x_l) + a[index];
}

void point(HDC hdc, double X, double Y) // ��������� ������ ����� �� ��������� �����������
{
    MoveToEx(hdc, X, Y, NULL);
    LineTo(hdc, X, Y);
}

void axises(HDC hdc, int X0, int Y0, int Width, int Height) { //������ ��� �������� � X0 Y0 (��� �����)

    //� �� ����, ��� ������ �������������� ����, ��� ��� ��������� ���������� ��� ��� � �����
    HPEN p1, p2 = CreatePen(PS_DASH, 0, RGB(0, 0, 0)); // ������� ����
    p1 = (HPEN)SelectObject(hdc, p2);  // ������� ���� � �������� ���������

    MoveToEx(hdc, X0, 0, NULL);
    LineTo(hdc, X0, Height);
    MoveToEx(hdc, 0, Y0, NULL);
    LineTo(hdc, Width, Y0);

    SelectObject(hdc, p1);   // ���������� ������ ����
    DeleteObject(p2);    // ������� ��������� ����
}

void cross(HDC hdc, double X, double Y) { //������ ������� ������� � ���� ������������ 

    HPEN p1, p2 = CreatePen(PS_SOLID, 1, RGB(255, 0, 0)); // ������� ����
    p1 = (HPEN)SelectObject(hdc, p2);  // ������� ���� � �������� ���������

    //������ �������
    MoveToEx(hdc, X + 7, Y + 7, NULL);
    LineTo(hdc, X - 7, Y - 7);
    MoveToEx(hdc, X - 7, Y + 7, NULL);
    LineTo(hdc, X + 7, Y - 7);

    SelectObject(hdc, p1);   // ���������� ������ ����
    DeleteObject(p2);    // ������� ��������� ����
}

void drawGraph(double f(double), double left, double right, bool crosses)
{
    HWND hwnd;
    char Title[1024];
    GetConsoleTitle(Title, 1024); // ������ ��� ����
    hwnd = FindWindow(NULL, Title); // ������ hwnd ����
    RECT rc;
    GetClientRect(hwnd, &rc);
    int Width = rc.right; //������ ����
    int Height = rc.bottom; //������ ����
    HDC hdc = GetDC(hwnd);  // �������� �������� ��� ���������

    int R = 0, G = 0, B = 0;
    if (crosses) B = 255; //����� ������� � ������ � ��� ���� ������� �����
    else G = 255;
    HPEN p1, p2 = CreatePen(PS_SOLID, 2, RGB(R, G, B)); // ������� ����
    p1 = (HPEN)SelectObject(hdc, p2);  // ������� ���� � �������� ���������

    double scaleX = 80, scaleY = 15; //�������� ������������ �� ����
    double x, X, Y;
    int X0 = Width * 0.7; // ������ ���������
    int Y0 = Height * 0.5; // ������ ��������� (������, ��� y �������������)
    axises(hdc, X0, Y0, Width, Height); //������ ���
    /*���� ���-�� �� �����, ����� ������� �� ������ ������� � ����� ��,
    �� ����� � ������ ������� ��������� � ������ ������, ������� ��, � ������������� ���������*/


    for (x = left; x <= right; x += 0.001)
    {
        X = scaleX * x + X0; //������� ����������, ���� ���������� �����
        Y = scaleY * f(x) + Y0; //����� �� ���������
        point(hdc, X, Y); //������ ����� (�� ����� ������� ������)
    }

    if (crosses) //���� �� ��������� ������� ���� ������������, �� ������ ��
    {
        for (int i = 0; i < n; i++) cross(hdc, scaleX * (left + h * i) + X0, scaleY * f(left + h * i) + Y0);
    }

    SelectObject(hdc, p1);   // ���������� ������ ����
    ReleaseDC(hwnd, hdc);    // ����������� �������� ���������
    DeleteObject(p2);       // ������� ��������� ����
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

    //��� ������ ������
    y_koef[0] = b[0];
    alpha_koef[0] = -_c[0] / y_koef[0];
    beta_koef[0] = y[0] / y_koef[0];

    //��� ����� 2 - (n-1)
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