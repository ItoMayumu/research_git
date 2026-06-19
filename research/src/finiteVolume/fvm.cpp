// fvm.cpp
#include "fvm.h"

namespace fvm {

fvMatrix laplacian(double k, volScalarField& psi) {
    // 1. 空っぽの行列計算機を作成（ここで psi が人質に取られる）
    fvMatrix matrix(psi);

    // 2. メッシュ情報の取得
    const fvMesh& mesh = psi.mesh();
    int Nx = mesh.Nx();
    int Ny = mesh.Ny();
    int Nz = mesh.Nz();
    int nCells = mesh.size();
    
    double dx = mesh.dx();
    double dy = mesh.dy();
    double dz = mesh.dz();

    // 3. 各方向の「熱の伝わりやすさ（コンダクタンス）」を計算
    // X方向の壁の面積は (dy * dz)、距離は dx
    double cx = k * (dy * dz) / dx;
    double cy = k * (dx * dz) / dy;
    double cz = k * (dx * dy) / dz;

    int Nxy = Nx * Ny;

    // 4. 全セルをループして、行列 A の係数を書き込む
    for (int i = 0; i < nCells; ++i) {
        
        // 現在の1次元インデックス(i)が、3次元空間のどこ(x,y,z)にいるかを逆算する
        // （壁の端っこにいるかどうかを判定するため）
        int x = i % Nx;
        int y = (i / Nx) % Ny;
        int z = i / Nxy;

        double aC_val = 0.0; // 中心の係数

        // 東 (X方向のプラス側)
        if (x + 1 < Nx) {
            matrix.aE()[i] = -cx; // ⬅️ マイナスにする
            aC_val += cx;         // ⬅️ プラスにする
        }
        // 西 (X方向のマイナス側)
        if (x - 1 >= 0) {
            matrix.aW()[i] = -cx; // ⬅️ マイナスにする
            aC_val += cx;         // ⬅️ プラスにする
        }
        // 北 (Y方向のプラス側)
        if (y + 1 < Ny) {
            matrix.aN()[i] = -cy; // ⬅️ マイナスにする
            aC_val += cy;         // ⬅️ プラスにする
        }
        // 南 (Y方向のマイナス側)
        if (y - 1 >= 0) {
            matrix.aS()[i] = -cy; // ⬅️ マイナスにする
            aC_val += cy;         // ⬅️ プラスにする
        }
        // 上 (Z方向のプラス側)
        if (z + 1 < Nz) {
            matrix.aT()[i] = -cz; // ⬅️ マイナスにする
            aC_val += cz;         // ⬅️ プラスにする
        }
        // 下 (Z方向のマイナス側)
        if (z - 1 >= 0) {
            matrix.aB()[i] = -cz; // ⬅️ マイナスにする
            aC_val += cz;         // ⬅️ プラスにする
        }

        
        
        matrix.aC()[i] = aC_val;
    }

    return matrix;
}

} 