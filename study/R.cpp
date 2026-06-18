#include <bits/stdc++.h>

using namespace std;

int main(){
    int a;
    cin >> a;

    vector<int> data(5);
    for(int i=0; i<5; i++){
        cin >> data[i];
    }

    func();
}

// 多重ループ

int func(){
    for(int i=0; i<5; i++){
        for(int j=0; j<5; j++){
            cout << i << " " << j << endl;
        }
    }
}

// 多次元配列
int vec(){
    vector<vector<int>>  data = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9}
    };
}

