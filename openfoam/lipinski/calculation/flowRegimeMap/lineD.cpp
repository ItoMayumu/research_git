#include <iostream>
#include <cmath>
#include <iomanip>
#include <vector>

int main() {
    // 1. 各種パラメータの設定 (25℃, 1気圧の空気と水)
    const double rho_f = 997.0;       // 液相密度 [kg/m^3]
    const double rho_g = 1.184;       // 気相密度 [kg/m^3]
    const double mu_f  = 8.90e-4;     // 液相粘度 [Pa*s]
    const double sigma = 0.07197;     // 表面張力 [N/m]
    const double g     = 9.80665;     // 重力加速度 [m/s^2]
    const double D     = 25.4e-3;     // 管径 [m] (25.4 mm)
    const double C0    = 1.2;         // 分布パラメータ (スラグ流の一般的な値として仮定)

    const double delta_rho = rho_f - rho_g;

    // 2. N_muf の計算 (式34)
    // N_muf = mu_f / [ rho_f * sigma * sqrt(sigma / (g * delta_rho)) ]^(1/2)
    double inner_sqrt = std::sqrt(sigma / (g * delta_rho));
    double N_muf = mu_f / std::sqrt(rho_f * sigma * inner_sqrt);

    // 3. 曲線Dの j_g を計算 (式35)
    // j_g = [ (sigma * g * delta_rho) / rho_g^2 ]^(1/4) * N_muf^(-0.2)
    double jg_term1 = std::pow((sigma * g * delta_rho) / (rho_g * rho_g), 0.25);
    double jg_term2 = std::pow(N_muf, -0.2);
    double jg_curveD = jg_term1 * jg_term2;

    // 4. 式(36)に基づく管径の適用条件チェック
    // D > sqrt(sigma / (delta_rho * g)) * N_muf^0.4 / [ (1 - 0.11*C0) / C0 ]^2
    double D_min_numerator = std::sqrt(sigma / (delta_rho * g)) * std::pow(N_muf, 0.4);
    double D_min_denominator = std::pow((1.0 - 0.11 * C0) / C0, 2);
    double D_min = D_min_numerator / D_min_denominator;

    // 結果のヘッダ出力
    std::cout << "=== 曲線D (スラグ流から環状噴霧流への遷移境界) ===" << std::endl;
    std::cout << std::fixed << std::setprecision(5);
    std::cout << "N_muf      : " << N_muf << std::endl;
    std::cout << "必要最小管径: " << D_min * 1000.0 << " [mm]" << std::endl;
    std::cout << "設定管径    : " << D * 1000.0 << " [mm]" << std::endl;
    
    if (D > D_min) {
        std::cout << " -> 管径の適用条件 (式36) を満たしています。" << std::endl;
    } else {
        std::cout << " -> [警告] 管径が適用条件を満たしていません。" << std::endl;
    }
    std::cout << "==================================================" << std::endl;

    // j_f に対する j_g の出力
    std::cout << "\n" << std::setw(12) << "j_f [m/s]" << " | " << std::setw(12) << "j_g [m/s]" << std::endl;
    std::cout << "---------------------------------" << std::endl;

    // j_f を 0.01 から 10 まで対数的に増加させて出力
    // j_g は j_f に依存しないため、常に同じ値が出力されます
    double jf = 0.01;
    while (jf <= 10.001) { // 浮動小数点の誤差を考慮して少し余裕を持たせる
        std::cout << std::setw(12) << jf << " | " << std::setw(12) << jg_curveD << std::endl;
        
        // 対数プロット用に値を更新 (例: 1, 2, 5 の倍数でステップ)
        if (jf < 0.019)      jf = 0.02;
        else if (jf < 0.049) jf = 0.05;
        else if (jf < 0.099) jf = 0.1;
        else if (jf < 0.19)  jf = 0.2;
        else if (jf < 0.49)  jf = 0.5;
        else if (jf < 0.99)  jf = 1.0;
        else if (jf < 1.9)   jf = 2.0;
        else if (jf < 4.9)   jf = 5.0;
        else if (jf < 9.9)   jf = 10.0;
        else                 break;
    }

    return 0;
}