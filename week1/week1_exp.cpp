#include <iostream>
#include <iomanip>
#include <cmath>
using namespace std;

unsigned long long factorial(int n){ //  for non-negative long long values
    unsigned long long factor = 1;
    for(int i = 1; i <= n; i++){
        factor *= i;
    }
    return factor;
}

double exp_taylor (double x, int Nterms){ // Nterms controls iter
    double sum = 1.0;
    for(int n = 1; n < Nterms; n++){
        sum += pow(x, n) / factorial(n);
    }
    return sum;
}

int main(){
    double x; cout << "appr value of x for e^x: "; cin >> x;
    int Nterms = 10; 
    cout << setprecision(15);

    double appr_exp = exp_taylor(x, Nterms);
    cout << "e^x appr = " << appr_exp << ", x = " << x; 

    double exact_exp = exp(x); cout << "\nexact_exp: " << exact_exp;
    double error = fabs(exact_exp - appr_exp); 
    cout << "\nerror: " << error; 
}  