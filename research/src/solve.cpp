#include <iostream>
using namespace std;

int main(){
    double T0=0;
    double T3=100;

    double T1=0;
    double T2=0;

    for(int i=0;i<3;i++){
        T1=(T0+T2)/2;
        T2=(T1+T3)/2;
        cout << "T1: " << T1 << ", T2: " << T2 << endl;
    }
}
