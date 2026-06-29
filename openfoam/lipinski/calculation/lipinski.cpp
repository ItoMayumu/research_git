#include <iostream>
#include <cmath>
#include <iomanip>

double calculateResidual(double alpha, double j_L, double j_G, double epsilon, double d_p,
                          double rho_L, double mu_L, double rho_G, double mu_G, double g){
                            
                            double common_denom = std::pow(epsilon, 3) * d_p;

                            double term_L_grav = rho_L * g;
                            double term_L_visc =(150.0 * mu_L * j_L * std::pow(1.0 - epsilon, 2)) / (std::pow(1 - alpha, 3) * common_denom * d_p);
                            double term_L_iner = (1.75 * (1.0 - epsilon) * rho_L * std::pow(j_L, 2)) / (std::pow(1.0 - alpha, 3) * common_denom);
                            double grad_P_L = term_L_grav + term_L_visc + term_L_iner;

                            double term_G_grav = rho_G * g;
                            double term_G_visc = (150.0 * mu_G * j_G * std::pow(1.0 - epsilon, 2)) / (std::pow(alpha, 3) * common_denom * d_p);
                            double term_G_iner = (1.75 * (1.0 - epsilon) * rho_G * std::pow(j_G, 2)) / (std::pow(alpha, 3) * common_denom);
                            double grad_P_G = term_G_grav + term_G_visc + term_G_iner;

                            return grad_P_L - grad_P_G;
                        }
int main(){
    double j_L = 0.105;
    double j_G = 0.050;
    double epsilon = 0.38;
    double d_p = 0.005;
    double g = 9.81;
    double rho_L = 998.2;     
    double mu_L = 1.002e-3;
    double rho_G = 1.205;
    double mu_G = 1.82e-5;  

    double alpha = 0.080;       // 初期仮定値
    double tolerance = 1e-6;  // 許容誤差 (Pa/m 単位での残差目標値)
    int max_iterations = 100; // 最大ループ回数
    double d_alpha = 1e-5;

    bool converged = false;
    for(int iter = 1; iter <= max_iterations; ++iter){
        double f = calculateResidual(alpha, j_L, j_G, epsilon, d_p, rho_L, mu_L, rho_G, mu_G, g);

        std::cout << std::setw(4) << iter << " | " 
                  << std::fixed << std::setprecision(6) << std::setw(8) << alpha << " | " 
                  << std::scientific << std::setprecision(4) << std::setw(22) << f << std::endl;


        if(std::abs(f) < tolerance){
            converged = true;
            break;
        }

        double f_plus = calculateResidual(alpha + d_alpha, j_L, j_G, epsilon, d_p, rho_L, mu_L, rho_G, mu_G, g);
        double df_dalpha = (f_plus - f) / d_alpha;

        alpha = alpha - f / df_dalpha;

        if (alpha <= 0.001) alpha = 0.001;
        if (alpha >= 0.999) alpha = 0.999;
    }

    std::cout << "--------------------------------------------------" << std::endl;
    if (converged) {
        // 確定したボイド率を用いて、最終的な一致した圧力損失を求める
        // 残差が0なので、液相・気相どちらの勾配を計算しても同じ値になる
        double common_denom = std::pow(epsilon, 3) * d_p;
        double final_grad_P = rho_L * g 
            + (150.0 * mu_L * j_L * std::pow(1.0 - epsilon, 2)) / (std::pow(1.0 - alpha, 3) * common_denom * d_p)
            + (1.75 * (1.0 - epsilon) * rho_L * std::pow(j_L, 2)) / (std::pow(1.0 - alpha, 3) * common_denom);

        std::cout << ">>> 収束成功 <<<" << std::endl;
        std::cout << "適合ボイド率 (alpha)   : " << std::fixed << std::setprecision(5) << alpha << " [-]" << std::endl;
        std::cout << "一致した圧力損失勾配    : " << std::fixed << std::setprecision(2) << final_grad_P / 1000.0 << " [kPa/m]" << std::endl;
    } else {
        std::cout << ">>> 警告: 設定したループ回数内に収束しませんでした <<<" << std::endl;
    }
    std::cout << "==================================================" << std::endl;

    double grad_P_L = 0.0;
    double grad_P_G = 0.0;
    double omega = 0.05;
    double delta = 1e-6;

    for(int iter = 1; iter <= max_iterations; ++iter){
        double eps3_dp = std::pow(epsilon, 3) * d_p;
        double eps3_dp2 = eps3_dp * d_p;

        // k_L, k_G, eta_L, eta_G の計算
        // 前回の式に基づき、透過率・パスアビリティを (1-alpha)^3, alpha^3 とみなしています
        double k_L = std::pow(1.0 - alpha, 3);
        double eta_L = std::pow(1.0 - alpha, 3);
        double k_G = std::pow(alpha, 3);
        double eta_G = std::pow(alpha, 3);

        // 液相の圧力損失勾配 ΔP_L / H_t (j_Lの抜けを修正済み)
        double term_L_grav = rho_L * g;
        double term_L_visc = (150.0 * mu_L * j_L * std::pow(1.0 - epsilon, 2)) / (k_L * eps3_dp2);
        double term_L_iner = (1.75 * (1.0 - epsilon) * rho_L * std::pow(j_L, 2)) / (eta_L * eps3_dp);
        grad_P_L = term_L_grav + term_L_visc + term_L_iner;

        // 気相の圧力損失勾配 ΔP_G / H_t
        double term_G_grav = rho_G * g;
        double term_G_visc = (150.0 * mu_G * j_G * std::pow(1.0 - epsilon, 2)) / (k_G * eps3_dp2);
        double term_G_iner = (1.75 * (1.0 - epsilon) * rho_G * std::pow(j_G, 2)) / (eta_G * eps3_dp);
        grad_P_G = term_G_grav + term_G_visc + term_G_iner;

        double relative_error = (grad_P_L - grad_P_G) / grad_P_L;

        if (iter <= 20 || iter % 100 == 0) {
             std::cout << std::setw(4) << iter << " | " 
                       << std::fixed << std::setprecision(6) << std::setw(8) << alpha << " | " 
                       << std::scientific << std::setprecision(4) << std::setw(14) << std::abs(relative_error) << std::endl;
        }
        if (std::abs(relative_error) < delta) {
            converged = true;
            std::cout << " ... (converged at iter " << iter << ")" << std::endl;
            break;
        }

        // --- ボイド率 α の更新（緩和法） ---
        alpha = alpha - omega * relative_error;
    }

    std::cout << "--------------------------------------------------" << std::endl;
    if (converged) {
        std::cout << ">>> 収束成功 <<<" << std::endl;
        std::cout << "適合ボイド率 (alpha)   : " << std::fixed << std::setprecision(5) << alpha << " [-]" << std::endl;
        // 収束していれば ΔP_L ≒ ΔP_G なので、代表して grad_P_L を表示 (Pa/m -> kPa/m)
        std::cout << "一致した圧力損失勾配    : " << std::fixed << std::setprecision(2) << grad_P_L / 1000.0 << " [kPa/m]" << std::endl;
    } else {
        std::cout << ">>> 警告: 設定したループ回数内に収束しませんでした <<<" << std::endl;
        std::cout << "緩和係数 omega の値を小さくするか、max_iterations を増やしてください。" << std::endl;
    }
    std::cout << "==================================================" << std::endl;

    return 0;
}