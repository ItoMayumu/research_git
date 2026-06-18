#include <bits/stdc++.h>
using namespace std;

int main() {

  for (int i = 0; i < 5; i++) {

    if (i == 3) {
      cout << "forとばす" << endl;
      continue;
    }

    cout << "for" << i << endl;
  }

  cout << "for終了" << endl;

  int j = 0;
  while(j < 5) {

    if (j == 3) {
      cout << "whileとばす" << endl;
      continue;
    }

    cout << "while" << j << endl;
    j++;
  }

  cout << "while終了" << endl;
}
