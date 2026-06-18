#include <iostream>

// これまで作った3種の神器＋魔法の翻訳機をすべて読み込む
#include "../src/fvMesh/fvMesh.h"
#include "../src/fields/volScalarField.h"
#include "../src/fvMatrix/fvMatrix.h"
#include "../src/finiteVolume/fvm.h"

int main() {
    std::cout << "=== Starting miniLaplacianFoam ===" << std::endl;

    // 1. 空間（メッシュ）の作成
    // 21 x 21 x 21 マス、サイズ 1.0m x 1.0m x 1.0m の空間
    fvMesh mesh(21, 21, 21, 1.0, 1.0, 1.0);

    // 2. 温度フィールドの作成（初期温度はすべて 0.0 ℃）
    volScalarField T("Temperature", mesh, 0.0);

    // 3. 熱伝導方程式の組み立て
    double k = 0.1; // 熱伝導率
    fvMatrix TEqn = fvm::laplacian(k, T);

    // 4. 熱源（ヒーター）の設定
    // 空間のど真ん中のセル番号を取得し、そこに強力な熱源(100.0)をセット
    int center_idx = mesh.index(10, 10, 10);
    TEqn.source()[center_idx] = 100.0;

    // 5. 方程式を解く！（ここでガウス・ザイデル法が走る）
    std::cout << "Solving the heat equation..." << std::endl;
    TEqn.solve();

    // 6. 結果の確認
    std::cout << "=== Result ===" << std::endl;
    
    // 中心（ヒーターの場所）の温度
    std::cout << "Center (10,10,10)      : " << T[center_idx] << " C" << std::endl;
    
    // 中心の1つ隣の温度
    int next_idx = mesh.index(11, 10, 10);
    std::cout << "Next to Center (11,10,10): " << T[next_idx] << " C" << std::endl;
    
    // 空間の角っこ（一番遠い場所）の温度
    int edge_idx = mesh.index(0, 0, 0);
    std::cout << "Edge (0,0,0)          : " << T[edge_idx] << " C" << std::endl;

    return 0;
}