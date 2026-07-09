#include <iostream>
#include <cmath>
#include <vector>
#include <iomanip>

double calculate_jf(double jg, double C_o = 1.2, double sigma = 0.0728, 
                    double delta_rho = 996.0, double rho_f = 998.0, double g = 9.81) {
    
    double term1 = (3.33 / C_o) - 1.0;
    
    double term2_inner = (sigma * g * delta_rho) / (rho_f * rho_f);
    double term2 = (0.76 / C_o) * std::pow(term2_inner, 0.25);
    
    double jf = term1 * jg - term2;
    
    return jf;
}

int main() {
    std::vector<double> jg_values = {0.1, 0.2,0.3,0.4, 0.5,0.6,0.7,0.8,0.9,1.0,1.1,1.2,1.3,1.4,1.5};
    
    std::cout << std::fixed << std::setprecision(4);
    
    for (double jg : jg_values) {
        double jf = calculate_jf(jg);
        std::cout << "jg = " << std::setprecision(1) << jg 
                  << " m/s のとき、jf = " << std::setprecision(4) << jf << " m/s\n";
    }
    
    return 0;
}