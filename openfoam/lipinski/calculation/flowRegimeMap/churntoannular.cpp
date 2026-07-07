#include <iostream>
#include <cmath>
#include <fstream>
#include <iomanip>

using namespace std;

// --- 物理定数および物性値（大気圧 空気-水系、25℃を想定） ---
const double D = 0.0254;                // 管径 [m]
const double g = 9.81;                  // 重力加速度 [m/s^2]
const double rho_f = 1000.0;            // 液相密度 [kg/m^3]
const double rho_g = 1.2;               // 気相密度 [kg/m^3]
const double delta_rho = rho_f - rho_g; // 密度差 [kg/m^3]
const double sigma = 0.072;             // 表面張力 [N/m] (水-空気)
const double C0 = 1.2;                  // 分布パラメータ

int main() {
    // CSVファイルの出力準備
    ofstream outFile("churn_to_annular_boundary.csv");
    if (!outFile) {
        cerr << "ファイルを開けませんでした。" << endl;
        return 1;
    }

    // ヘッダーの書き込み
    outFile << "j_g [m/s],j_f [m/s],alpha" << endl;

    // ドリフト速度 V_gj の計算（論文の式29を使用）
    double V_gj = 0.35 * sqrt((delta_rho * g * D) / rho_f);
    
    // ※ 高ボイド率領域として式(37)を使用する場合は以下のコメントアウトを外して切り替えます
    // double V_gj = sqrt(2.0) * pow((sigma * g * delta_rho) / (rho_f * rho_f), 0.25);

    cout << "計算を開始します..." << endl;

    // j_g を 1.0 m/s から 30.0 m/s までスイープして j_f を計算
    for (double j_g = 1.0; j_g <= 30.0; j_g += 0.5) {
        
        // 1. 式(32)からボイド率 α を計算
        // j_g = sqrt(Δρ*g*D / ρg) * (α - 0.11) を α について解いた形
        double alpha = 0.11 + j_g * sqrt(rho_g / (delta_rho * g * D));

        // ボイド率 α が 1.0 以上になるのは物理的にあり得ないため除外
        if (alpha >= 1.0) {
            continue; 
        }

        // 2. ドリフトフラックスモデルから j_f を逆算
        // α = j_g / (C0*(j_g + j_f) + V_gj) を j_f について解いた形
        double j_f = (j_g / (alpha * C0)) - (V_gj / C0) - j_g;

        // j_f がマイナスになる領域はプロット対象外として出力しない
        if (j_f > 0.0) {
            outFile << fixed << setprecision(4) << j_g << "," << j_f << "," << alpha << endl;
        }
    }

    outFile.close();
    cout << "完了しました。'churn_to_annular_boundary.csv' を出力しました。" << endl;

    return 0;
}