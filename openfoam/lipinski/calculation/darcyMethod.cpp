#include <iostream>
#include <cmath>
#include <iomanip>

struct PorousCoeffs {
    double D; 
    double F; 
};

int main() {
    double epsilon = 0.38; // 空隙率
    double alpha = 0.08;   // ボイド率
    double dp = 0.005;     // 粒子径 [m]

    double eps3 = std::pow(epsilon, 3);

    // --- 1. 液相 (Liquid: 水) の計算 ---
    // 見かけ流速への変換により、分母の累乗が約分される
    double denom_D_L = std::pow(1.0 - alpha, 2) * eps3; // (1-alpha)^2
    double denom_F_L = (1.0 - alpha) * eps3;            // (1-alpha)^1
    
    PorousCoeffs liquid;
    liquid.D = 150.0 * std::pow(1.0 - epsilon, 2) / (denom_D_L * std::pow(dp, 2));
    liquid.F = 3.5 * (1.0 - epsilon) / (denom_F_L * dp);

    // --- 2. 気相 (Gas: 空気) の計算 ---
    // 見かけ流速への変換により、分母の累乗が約分される
    double denom_D_G = std::pow(alpha, 2) * eps3; // alpha^2
    double denom_F_G = alpha * eps3;              // alpha^1
    
    PorousCoeffs gas;
    gas.D = 150.0 * std::pow(1.0 - epsilon, 2) / (denom_D_G * std::pow(dp, 2));
    gas.F = 3.5 * (1.0 - epsilon) / (denom_F_G * dp);

    // 結果出力
    std::cout << std::scientific << std::setprecision(2);
    std::cout << "【修正版】水（液相）の係数" << std::endl;
    std::cout << "  D_L = " << liquid.D << " [1/m^2]" << std::endl;
    std::cout << "  F_L = " << liquid.F << " [1/m]\n" << std::endl;

    std::cout << "【修正版】空気（気相）の係数" << std::endl;
    std::cout << "  D_G = " << gas.D << " [1/m^2]" << std::endl;
    std::cout << "  F_G = " << gas.F << " [1/m]" << std::endl;

    return 0;
}