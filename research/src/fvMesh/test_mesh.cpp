// test_mesh.cpp
#include <iostream>
#include "fvMesh.h" // ここで自作のヘッダーを読み込む！

int main() {
    // 21 x 21 x 21 マスで、1辺が 1.0 m の3D空間を作る
    fvMesh mesh(21, 21, 21, 1.0, 1.0, 1.0);

    std::cout << "--- fvMesh Test ---" << std::endl;
    std::cout << "Total Cells : " << mesh.size() << std::endl;
    std::cout << "Cell Volume : " << mesh.V() << " m^3" << std::endl;
    std::cout << "dx          : " << mesh.dx() << " m" << std::endl;
    
    // 真ん中のセルのインデックス番号を確認
    int center_idx = mesh.index(10, 10, 10);
    std::cout << "Center Index: " << center_idx << std::endl;

    return 0;
}