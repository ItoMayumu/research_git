#include "../src/fvMesh/fvCylindricalMesh.h"
#include "../src/fields/volScalarFieldFluid.h" // 🌟 作成したベクトルフィールドを読み込む
#include <iostream>
#include <fstream>
#include <cmath>
#include <string>

int main() {
    // 1. メッシュの生成
    int Nr = 8, Ntheta = 24, Nx = 5;
    double R = 0.05, Theta = 2.0 * M_PI, Lx = 0.5;
    fvCylindricalMesh mesh(Nr, Ntheta, Nx, R, Theta, Lx);

    volVectorField U(std::string("U"), mesh);

    // 3. 適当な流れ場（例えば軸方向への押し出し流れ）をセットしてみる
    for (int i = 0; i < U.size(); ++i) {
        U[i] = {1.0, 0.0, 0.0}; // 全セルで X方向に 1.0 m/s の流速を代入
    }

    // 4. VTK出力
    U.writeVTK("flow_field.vtk");

    std::cout << "Successfully exported flow_field.vtk" << std::endl;
    return 0;
}