//配列

#include <bits/stdc++.h>
using namespace std;

int main(){
    string str;

    str = "abcd";

    cout << str.at(0) << endl;
    cout << str.at(1) << endl;
    cout << str.size() << endl;

    vector<int> vec;

    vec = { 25,100,300};
    cout << vec.at(0) << endl;
    cout << vec.size()  << endl;

    int N;
    cin >> N;
    vector<int> math(N);
    vector<int> english(N);

    for (int i = 0; i < N; i++){
        cin >> math.at(i);
    }

    for(int i= 0; i < N; i++){
        cin >> english.at(i);
    }

    for (int i = 0; i < N; i++) {
    cout << math.at(i) + english.at(i) << endl;
  }

}