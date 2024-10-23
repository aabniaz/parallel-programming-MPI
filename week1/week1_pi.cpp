#include <iostream>
#include <iomanip> 
#include <cmath>
using namespace std;

double leibniz_pi(int number) {
    double pi = 0.0;
    for (int n = 0; n < number; ++n) {
        double term = pow(-1, n) / (2 * n + 1);
        pi += term;
    }
    pi *= 4; 
    return pi;
}

int main() {
    int number=10000;
    // cin >> number;
    cout << setprecision(15); 

    double approx_pi = leibniz_pi(number);
    cout << "pi_appr = " << approx_pi;;

    double pi_exact = M_PI; cout << "\npi_exact = " << pi_exact;
    cout << "\nerror: " << abs(pi_exact-approx_pi);
}