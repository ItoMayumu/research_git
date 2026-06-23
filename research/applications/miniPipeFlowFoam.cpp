#include "../src/fvMesh/fvCylindricalMesh.h"
#include <iostream>
#include <fstream>
#include <cmath>

int main() {
    std::cout << "=== Cylindrical Mesh Generator ===" << std::endl;

    // 🌟 ここで好きな分割数を指定します！
    // 半径方向(Nr)=8, 周方向(Ntheta)=24, 軸方向(Nx)=5
    // 最大半径=0.05m, 角度=2π, パイプ長さ=0.5m
    int Nr = 8;
    int Ntheta = 24;
    int Nx = 5;
    double R = 0.05;
    double Theta = 2.0 * M_PI;
    double Lx = 0.5;

    fvCylindricalMesh mesh(Nr, Ntheta, Nx, R, Theta, Lx);

    // 座標を書き出すファイルを開く
    std::ofstream os("mesh_points.txt");
    
    // 全マスをループして、各マスの中心の「3次元デカルト座標(X,Y,Z)」を計算して保存
    for (int ix = 0; ix < mesh.Nx(); ++ix) {
        for (int itheta = 0; itheta < mesh.Ntheta(); ++itheta) {
            for (int ir = 0; ir < mesh.Nr(); ++ir) {
                
                // 円筒座標におけるセルの中心位置 (r, theta, x) を計算
                double r     = (ir + 0.5) * mesh.dr();
                double theta = (itheta + 0.5) * mesh.dtheta();
                double x     = ix * mesh.dx();

                // 🌟 Python(Matplotlib)で描画できるように、なじみのあるX, Y, Z座標に変換！
                double abs_x = x;
                double abs_y = r * std::cos(theta);
                double abs_z = r * std::sin(theta);

                // ファイルに 1行ずつスペース区切りで保存
                os << abs_x << " " << abs_y << " " << abs_z << "\n";
            }
        }
    }

    std::cout << "Successfully exported mesh coordinates to mesh_points.txt" << std::endl;
    return 0;
}