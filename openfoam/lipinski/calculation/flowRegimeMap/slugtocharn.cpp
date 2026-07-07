#include <iostream>
#include <cmath>
#include <fstream>
#include <iomanip>

using namespace std;

// --- 物理定数および物性値（大気圧 空気-水系を想定） ---
const double D = 0.0254;                // 管径 [m]
const double g = 9.81;                  // 重力加速度 [m/s^2]
const double rho_f = 1000.0;            // 液相密度 [kg/m^3]
const double rho_g = 1.2;               // 気相密度 [kg/m^3]
const double delta_rho = rho_f - rho_g; // 密度差 [kg/m^3]
const double nu_f = 1.0e-6;             // 液相動粘度 [m^2/s]
const double C0 = 1.2;                  // 分布パラメータ

// --- 計算の高速化のための共通項の事前計算 ---
// V_gj 項: 0.35 * sqrt(Δρ*g*D / ρf)
const double V_gj = 0.35 * sqrt((delta_rho * g * D) / rho_f);

// N_mu 項: (Δρ*g*D^3 / (ρf * νf^2))^(1/18)
const double N_mu = pow((delta_rho * g * pow(D, 3)) / (rho_f * pow(nu_f, 2)), 1.0 / 18.0);

// 式(28)右辺分母の終端速度係数項: 0.75 * sqrt(Δρ*g*D/ρf) * N_mu
const double V_term2 = 0.75 * sqrt((delta_rho * g * D) / rho_f) * N_mu;

// --- 遷移条件の残差を計算する関数 ---
// 式(28)と式(29)が等しくなるとき(lhs - rhs = 0)が境界線
double calculate_residual(double j_g, double j_f) {
    double j = j_f + j_g; // 混合見かけ速度

    // 左辺(LHS): 式(29) ドリフトフラックスモデルによるボイド率 α
    double lhs = j_g / (C0 * j + V_gj);

    // 右辺(RHS): 式(28) スラグ気泡部の平均ボイド率 α_m
    double inner_term = ((C0 - 1.0) * j + V_gj) / (j + V_term2);
    // inner_termが負になることは物理的に想定されませんが、念のためガード
    if (inner_term < 0.0) inner_term = 0.0; 
    
    double rhs = 1.0 - 0.813 * pow(inner_term, 0.75);

    return lhs - rhs;
}

// --- 二分法による j_f の探索 (変更箇所) ---
double find_jf(double j_g) {
    double jf_low = 0.0001; // j_f の探索下限 [m/s]
    double jf_high = 100.0; // j_f の探索上限 [m/s]
    double tol = 1e-6;      // 許容誤差

    // 解が区間内に存在するかチェック
    if (calculate_residual(j_g, jf_low) * calculate_residual(j_g, jf_high) > 0) {
        return -1.0; // エラー: 根が見つからない
    }

    // 二分法ループ
    while ((jf_high - jf_low) > tol) {
        double jf_mid = (jf_low + jf_high) / 2.0;
        if (calculate_residual(j_g, jf_mid) * calculate_residual(j_g, jf_low) <= 0) {
            jf_high = jf_mid;
        } else {
            jf_low = jf_mid;
        }
    }
    return (jf_low + jf_high) / 2.0;
}

int main() {
    // CSVファイルの出力準備
    ofstream outFile("transition_boundary_jg_based.csv");
    if (!outFile) {
        cerr << "ファイルを開けませんでした。" << endl;
        return 1;
    }

    // ヘッダーの書き込み (j_gを先に出力するフォーマットに変更)
    outFile << "j_g [m/s],j_f [m/s]" << endl;

    cout << "計算を開始します..." << endl;

    // j_g を 0.09 m/s から 13.0 m/s までスイープして j_f を計算 (変更箇所)
    // 刻み幅は滑らかなグラフになるように 0.1 としています
    for (double j_g = 0.09; j_g <= 13.0; j_g += 0.1) {
        double j_f = find_jf(j_g);
        
        if (j_f > 0) {
            outFile << fixed << setprecision(4) << j_g << "," << j_f << endl;
        } else {
            cerr << "j_g = " << j_g << " において解が収束しませんでした。" << endl;
        }
    }

    outFile.close();
    cout << "完了しました。'transition_boundary_jg_based.csv' を出力しました。" << endl;

    return 0;
}