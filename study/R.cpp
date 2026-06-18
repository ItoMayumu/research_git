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

    // 宣言
    vector<vector<int>> data2(3, vector<int>(3));
    vector<vector<vector<int>>> data3(3, vector<vector<int>> (3, vector<int>(3)));
}


// 参照
int  view(){
    int a=3;
    int &b = a; // 参照

    
}