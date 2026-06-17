// while(条件式){
// 
// }

#include <bits/stdc++.h>
using namespace std;

int main(){
    int N;
    cin >> N;

    int sum = 0;
    int x;
    int i = 0;




    while (i<N){
        cin >> x;
        sum += x;
        i++;
    }

    cout << sum << endl;
}