#include <iostream>
#include <cmath>  
using namespace std;

double f(double x) {
    return 2 * x * x - sqrt(x + 2);
}

double integ_simpson(double a, double b, int n) {
    if (n % 2 != 0) {
        cout << "n must be even" << endl;
        return -1;
    }
    double h = (b - a) / n; //shag setki
    double sum = f(a);     
    double current_point = a;
    for (int i = 1; i < n; i++) {
        current_point += h;
        double temp = f(current_point);
        if (i % 2 != 0)
            sum += 4 * temp; //for odd ind
        else
            sum += 2 * temp; //for even ind
    }
    sum += f(b); 
    return (h / 3) * sum;
}

int main() {
    int n; cout << "enter n: "; cin >> n; //more n, more accuracy, ex: n=30
    double a, b; cout << "enter range [a, b]: "; cin >> a >> b;
    cout.precision(12);

    double result = integ_simpson(a, b, n);
    if (result != -1) {
        cout << "numerical  integration: " << result << endl;
    }

    double exact = 38.2020410288673; //for example when a -2 b 4
    cout << "error: " << abs(exact-result);
    return 0;
}