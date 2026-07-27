#include <iostream>
#include <cmath>
#include <fstream>
#include <iomanip>

using namespace std;

const double D = 0.0254;                
const double g = 9.81;                  
const double rho_f = 1000.0;            
const double rho_g = 1.2;               
const double delta_rho = rho_f - rho_g; 
const double sigma = 0.072;            
const double C0 = 1.2;                 

int main() {
    ofstream outFile("churn_to_annular_boundary.csv");
    if (!outFile) {
        cerr << "ファイルを開けませんでした。" << endl;
        return 1;
    }
    outFile << "j_g [m/s],j_f [m/s],alpha" << endl;

    double V_gj = 0.35 * sqrt((delta_rho * g * D) / rho_f);

    cout << "計算を開始します..." << endl;

    for (double j_g = 1.0; j_g <= 30.0; j_g += 0.5) {
        double alpha = 0.11 + j_g * sqrt(rho_g / (delta_rho * g * D));
        if (alpha >= 1.0) {
            continue; 
        }

        double j_f = (j_g / (alpha * C0)) - (V_gj / C0) - j_g;

        if (j_f > 0.0) {
            outFile << fixed << setprecision(4) << j_g << "," << j_f << "," << alpha << endl;
        }
    }

    outFile.close();
    cout << "完了しました。'churn_to_annular_boundary.csv' を出力しました。" << endl;

    return 0;
}