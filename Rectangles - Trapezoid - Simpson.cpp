#define _CRT_SECURE_NO_WRARNINGS
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <vector>
//по Рунге (читайте просто википедию первый вбабзац) мы каждый раз оцениваем точность вычисленного интеграла (в условии при while в каждой функции)
//но такая оценка точности будет рабочей только для дважды дифференцируемых функций

double l_rectangle(double x_l, double x_r);
double c_rectangle(double x_l, double x_r);
double trapezoid(double x_l, double x_r);
double simpson(double x_l, double x_r);

using namespace std;
double eps = 0.001;
int n;

double f(double x) {
	//return x;
	return 2 * sin(x) + pow(x, 2) / 10 + 2;
	//return pow(x, 5) + 2 * x + 10 * pow(x, 2);
}

double F(double x) {
	//return pow(x, 2) / 2;
	return -2 * cos(x) + pow(x, 3) / 30 + 2 * x;
	//return pow(x_r, 6)/6 + pow(x_r, 2) + (10/3) * pow(x_r, 3);
}

double integral(double x_l, double x_r) {
	return F(x_r) - F(x_l);
}

int main() {
	double x_l = -1, x_r = 1;
	double tmp, tmpint;
	tmpint = integral(x_l, x_r);
	cout << "Definite integral:\t" << tmpint << "\t" << 0 << "\n";
	tmp = l_rectangle(x_l, x_r);
	cout << "Left rectangles:\t" << tmp << "\t" << abs(tmp - tmpint) << "\t" << n << "\n";
	tmp = c_rectangle(x_l, x_r);
	cout << "Central rectangles:\t" << tmp << "\t" << abs(tmp - tmpint) << "\t" << n << "\n";
	tmp = trapezoid(x_l, x_r);
	cout << "Trapezoids:\t\t" << tmp << "\t" << abs(tmp - tmpint) << "\t" << n << "\n";
	tmp = simpson(x_l, x_r);
	cout << "Simpson:\t\t" << tmp << "\t" << abs(tmp - tmpint) << "\t" << n << "\n";

	system("pause");
}

//единственное, чем отличаются эти четыре функции: значение teta и строчка for (ну формула другая тупа)
double l_rectangle(double x_l, double x_r) {
	double teta = (double)1 / 3;
	n = 1;
	double length = abs(x_r - x_l);
	double s_n = 0;
	double s_2n = 0;
	do { // в каждой итерации этого цикла приближенно считаем интеграл при разбиении отрезка на n частей (каждый раз n возрастает вдвое)
		double s = 0;
		double sublength = length / n;
		for (int it = 0;it < n;it++) s += f(x_l + it * sublength) * sublength;
		s_n = s_2n;
		s_2n = s;
		n *= 2;
	} while (teta * abs(s_2n - s_n) > eps); // ну и если по Рунге мы получили достаточную точность при таком n, то можно заканчивать

	return s_2n;
}

double c_rectangle(double x_l, double x_r) {
	double teta = (double)1 / 3;
	n = 1;
	double length = abs(x_r - x_l);
	double s_n = 0;
	double s_2n = 0;
	do {
		double s = 0;
		double sublength = length / n;
		for (int it = 0;it < n;it++) s += f(x_l + it * sublength + 0.5 * sublength) * sublength;
		s_n = s_2n;
		s_2n = s;
		n *= 2;
	} while (teta * abs(s_2n - s_n) > eps);

	return s_2n;
}

double trapezoid(double x_l, double x_r) {


	double teta = (double)1 / 3;
	n = 1;
	double length = abs(x_r - x_l);
	double s_n = 0;
	double s_2n = 0;
	do {
		double s = 0;
		double sublength = length / n;
		for (int it = 0;it < n;it++) s += (f(x_l + sublength * it) + f(x_l + sublength * it + sublength)) / 2 * sublength;
		s_n = s_2n;
		s_2n = s;
		n *= 2;
	} while (teta * abs(s_2n - s_n) > eps);

	return s_2n;
}

double simpson(double x_l, double x_r) {
	double teta = (double)1 / 15;
	n = 1;
	double length = abs(x_r - x_l);
	double s_n = 0;
	double s_2n = 0;
	do {
		double s = 0;
		double sublength = length / n;
		for (int it = 0;it < n;it++) s += (sublength / 6) * (f(x_l + sublength * it) + f(x_l + sublength * it + sublength / 2) * 4 + f(x_l + sublength * it + sublength));
		s_n = s_2n;
		s_2n = s;
		n *= 2;
	} while (teta * abs(s_2n - s_n) > eps);

	return s_2n;
}