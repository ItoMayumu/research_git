#include <iostream>

#include "../src/fvMesh/fvMesh.h"
#include "../src/fields/volScalarField.h"
#include "../src/fvMatrix/fvMatrix.h"
#include "../src/finiteVolume/fvm.h"

int main() {
    std::cout << "=== Starting miniLaplacianFoam ===" << std::endl;

    fvMesh mesh(21, 21, 21, 1.0, 1.0, 1.0);

    volScalarField T("Temperature", mesh, 0.0);

    double k = 0.1; // 熱伝導率
    fvMatrix TEqn = fvm::laplacian(k, T);

    int center_idx = mesh.index(10, 10, 10);
    TEqn.source()[center_idx] = 100.0;

    // 4.5 境界条件（壁を氷水 0.0℃ で冷やす）
    // 空間の一番外側のセルを 0.0℃ に固定します（ペナルティ法）
    double huge = 1e15; // 圧倒的に巨大な数字
    int Nx = 21, Ny = 21, Nz = 21;
    for (int i = 0; i < mesh.size(); ++i) {
        int x = i % Nx;
        int y = (i / Nx) % Ny;
        int z = i / (Nx * Ny);
        
        // もし壁際（x, y, zのどれかが端っこ）なら
        if (x == 0 || x == Nx - 1 || y == 0 || y == Ny - 1 || z == 0 || z == Nz - 1) {
            // 行列の自分の係数と右辺を「巨大な数字」で上書きする
            TEqn.aC()[i] = huge;
            TEqn.source()[i] = huge * 0.0; // 0.0℃に固定！
        }
    }




    std::cout << "Solving the heat equation..." << std::endl;
    TEqn.solve();

    std::cout << "=== Result ===" << std::endl;
    
    std::cout << "Center (10,10,10)      : " << T[center_idx] << " C" << std::endl;
    
    int next_idx = mesh.index(11, 10, 10);
    std::cout << "Next to Center (11,10,10): " << T[next_idx] << " C" << std::endl;
    
    int edge_idx = mesh.index(0, 0, 0);
    std::cout << "Edge (0,0,0)          : " << T[edge_idx] << " C" << std::endl;

    return 0;
}