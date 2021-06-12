#define _CRT_SECURE_NO_WRARNINGS
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>

double bisection(double x_l, double x_r);
double simple_iterations(double x_l, double x_r);
double newton(double x_l, double x_r);
double relax(double x_l, double x_r);
double myMax(double x_l, double x_r);
double myMin(double x_l, double x_r);
double deriv(double x);
double koef(double x_l, double x_r);

using namespace std;
double eps = 0.0001;
int counter;

double f(double x) {
	return 2 * sin(x) + pow(x, 2) / 10 - 1;
	//return pow(x, 5) + 2 * x + 10 * pow(x, 2);
}

int main() {
	double x_l = -1, x_r = 3;
	double x_max = myMax(x_l, x_r);
	double x_min = myMin(x_l, x_r);

	cout << "Bisection: " << bisection(x_l, x_r) << " in " << counter << " iterations \n";
	cout << "Simple iterations: " << simple_iterations(x_l, x_r) << " in " << counter << " iterations \n";
	cout << "Newton: " << newton(x_l, x_r) << " in " << counter << " iterations \n";
	cout << "Relax: " << relax(x_l, x_r) << " in " << counter << " iterations \n";
	cout << "x_max from [" << x_l << ", " << x_r << "] = " << x_max << ", f(x_max) = " << f(x_max) << "\n";
	cout << "x_min from [" << x_l << ", " << x_r << "] = " << x_min << ", f(x_min) = " << f(x_min) << "\n";
	system("pause");
}

double deriv(double x) { //тупа через малое приращение вправо считает тангенс угла наклона секущей, поэтому на некоторых функция может быть критически неточной
	const double mtDx = 1.0e-6;

	double x1 = f(x + mtDx);
	double x0 = f(x);

	return (x1 - x0) / mtDx;
}

double bisection(double x_l, double x_r) { //при eps *= 0.1 количество итераций увеличивает на 3-4 (точне на log[2](10)) 
	double x_m;
	counter = 0;
	do {
		if (abs(f(x_l)) < eps) return x_l;
		if (abs(f(x_r)) < eps) return x_r;
		x_m = (x_l + x_r) / 2;
		if (f(x_l) * f(x_m) < 0)
			x_r = x_m;
		else
			x_l = x_m;
		counter++;
	} while (x_r - x_l > eps);
	return x_m;
}

double simple_iterations(double x_l, double x_r) {
	counter = 0;
	double k = 2.0 / (myMax(x_l, x_r) + 0.25);  //взято такое значение k, при котором почти точно будет сходимость метода
tryAgain:
	double x, xNew = (x_l + x_r) / 2;
	do {
		x = xNew;
		xNew = x - k * f(x);
		counter++;
	} while (abs(x - xNew) > eps);

	if (xNew<x_l || xNew>x_r) {
		k = -k;
		goto tryAgain;
	}
	return xNew;
}

double newton(double x_l, double x_r) {
	counter = 0;
	double x, xNew = (x_l + x_r) / 2 + 0.25;
	do {
		x = xNew;
		xNew = x - f(x) / deriv(x);  //взято такое значение k, при котором почти точно будет сходимость метода
		counter++;
	} while (abs(x - xNew) > eps);
	return xNew;
}

double relax(double x_l, double x_r) {
	counter = 0;
	double k = 2 / koef(x_l, x_r); //взято такое значение k, при котором почти точно будет сходимость метода (вроде даже наиболее оптимальное)
	double x, xNew = (x_l + x_r) / 2 + 1000 * eps;
	do {
		x = xNew;
		xNew = x - k * f(x);
		counter++;
	} while (abs(x - xNew) > eps);
	return xNew;
}

double koef(double x_l, double x_r) { //это тупа сумма max и min на отрезке, которые считаются по значению функции производной в 1000 точек
	double* arr = new double[1000];
	double length = x_r - x_l;
	for (int i = 0; i < 1000;i++) {
		arr[i] = deriv(x_l + i * length / 1000);
	}
	double min = arr[0], max = arr[0];
	for (int i = 0; i < 1000;i++) {
		if (max < arr[i]) max = arr[i];
		if (min > arr[i]) min = arr[i];
	}
	return max;
}

//эта "алгоритм имитации отжига" (изначально работает на всей области определения), который крайне неоптимально присопосблен к работе на отрезке
//про сам алгоритм на вики просто почитать можно
double myMin(double x_l, double x_r) {
	double* sols = new double[10]; //сюда запишем 10 значений, которые получим после 10 запусков "алгортма имитации отжига", чтобы найти среднее
	time_t systime; //для работы рандомма
	time(&systime); //для работы рандомма
	srand((unsigned int)systime); //для работы рандомма
	double alpha = 0.9; //скорость убывания температуры
	double length = x_r - x_l;
	double x = (x_l + x_r) / 2;
	double L = f(x);
	for (int i = 0;i < 10; i++) { // 10 раз прогоним "алгортма имитации отжига"
		for (double T = 80; T > 0.00008; T *= alpha)
		{
			for (int i = 0; i < 500; i++)
			{
			tryAgain:
				double xNew = x + (length / 10) * ((rand() / (double)RAND_MAX) * 2 - 1);
				if (xNew < x_l || xNew > x_r) goto tryAgain; //все просто: если при случайной модификации точки x в случайное xNew 
															 //последняя вышла за пределы рабочего отрзка, то модифицируем x в другую xNew еще раз
				double LNew = f(xNew);						 

				if (LNew < L || (rand() / (double)RAND_MAX) <= exp(-(LNew - L) / T))
				{
					L = LNew;
					x = xNew;
				}
			}
		}
		sols[i] = x;
	}
	x = 0;
	for (int i = 0;i < 10; i++) x += sols[i];
	x = x / 10; //посчитали среднее из 10 решений

	//поскольку такой алгоритм, основынный на случайности, почти никогда не будет попадать на концы отрезка, а высока вероятность, 
	//что наибольшее значение именно там, стоит сделать проверку, не находится ли этот максимум дна одном из концов
	sols[0] = x_l; //просто костыльно юзаем уже существующй массив, чтобы записать тада значения на концах отрезка
	sols[1] = x_r;
	for (int i = 0; i < 2;i++) {
		if (f(x) > f(sols[i])) x = sols[i];
	}
	return x;
}

double myMax(double x_l, double x_r) { //абсолютно аналогично предыдущему
	double* sols = new double[10];
	time_t systime;
	time(&systime);
	srand((unsigned int)systime);
	double alpha = 0.9;
	double length = x_r - x_l;
	double x = (x_l + x_r) / 2;
	double L = -f(x);
	for (int i = 0;i < 10; i++) {
		for (double T = 80; T > 0.00008; T *= alpha)
		{
			for (int i = 0; i < 500; i++)
			{
			tryAgain:
				double xNew = x + (length / 10) * ((rand() / (double)RAND_MAX) * 2 - 1);
				if (xNew < x_l || xNew > x_r) goto tryAgain;
				double LNew = -f(xNew);

				if (LNew < L || (rand() / (double)RAND_MAX) <= exp(-(LNew - L) / T))
				{
					L = LNew;
					x = xNew;
				}
			}
		}
		sols[i] = x;
	}
	x = 0;
	for (int i = 0;i < 10; i++) x += sols[i];
	x = x / 10;
	sols[0] = x_l;
	sols[1] = x_r;
	for (int i = 0; i < 2;i++) {
		if (f(x) < f(sols[i])) x = sols[i];
	}
	return x;
}