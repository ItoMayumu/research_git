#include <iostream>

int main(){
    std::cout << "start..." ;

    double huge_array[10000000];
    
    huge_array[0] = 1.23;
    huge_array[9999999] = 4.56;

    std::cout << "無事に配列にアクセスできました！: " << huge_array[0] << "\n";

    return 0;
}