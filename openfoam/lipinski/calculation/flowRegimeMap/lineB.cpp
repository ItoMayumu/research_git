#include <iostream>
#include <cmath>
#include <vector>
#include <stdexcept>
#include <fstream>

// -----------------------------
// 流体・管路の物性値
// -----------------------------
struct FluidParams {
    double rho_f;
    double rho_g;
    double nu_f;
    double D;
    double g = 9.81;

    double drho() const { return rho_f - rho_g; }
};

double computeC0(const FluidParams& p) {
    return 1.2 - 0.2 * std::sqrt(p.rho_g / p.rho_f);
}

double computeB(const FluidParams& p) {
    return std::sqrt(p.drho() * p.g * p.D / p.rho_f);
}

double computeA(const FluidParams& p) {
    double val = p.drho() * p.g * std::pow(p.D, 3) / (p.rho_f * p.nu_f * p.nu_f);
    return std::pow(val, 1.0 / 18.0);
}

// F(jf, jg) = LHS - RHS
double transitionEquation(double jf, double jg, const FluidParams& p) {
    double C0 = computeC0(p);
    double b  = computeB(p);
    double a  = computeA(p);
    double j  = jf + jg;

    double LHS = jg / (C0 * j + 0.35 * b);

    double numerator   = (C0 - 1.0) * j + 0.35 * b;
    double denominator = j + 0.75 * a * b;

    double ratio = numerator / denominator;
    if (ratio < 0.0) ratio = 0.0;

    double RHS = 1.0 - 0.813 * std::pow(ratio, 0.75);

    return LHS - RHS;
}

// -----------------------------
// jg方向に対数刻みでスキャンし、符号反転する区間を自動で探す
// -----------------------------
bool findBracket(double jf, const FluidParams& p,
                  double jg_min, double jg_max, int nPoints,
                  double& jg_lo, double& jg_hi)
{
    double logMin = std::log10(jg_min);
    double logMax = std::log10(jg_max);
    double dlog = (logMax - logMin) / (nPoints - 1);

    double prevJg = std::pow(10.0, logMin);
    double prevF  = transitionEquation(jf, prevJg, p);

    for (int i = 1; i < nPoints; ++i) {
        double curJg = std::pow(10.0, logMin + i * dlog);
        double curF  = transitionEquation(jf, curJg, p);

        if (prevF * curF < 0.0) {
            jg_lo = prevJg;
            jg_hi = curJg;
            return true;
        }
        prevJg = curJg;
        prevF  = curF;
    }
    return false;
}

// -----------------------------
// 二分法でjgを求める(jfを与える)
// -----------------------------
double solveJg(double jf, const FluidParams& p,
               double jg_min = 1e-4, double jg_max = 1e3,
               int scanPoints = 400,
               double tol = 1e-10, int maxIter = 200)
{
    double jg_lo, jg_hi;
    if (!findBracket(jf, p, jg_min, jg_max, scanPoints, jg_lo, jg_hi)) {
        throw std::runtime_error(
            "No sign change found in jg range - widen jg_min/jg_max.");
    }

    double f_lo = transitionEquation(jf, jg_lo, p);

    double jg_mid = jg_lo;
    for (int iter = 0; iter < maxIter; ++iter) {
        jg_mid = 0.5 * (jg_lo + jg_hi);
        double f_mid = transitionEquation(jf, jg_mid, p);

        if (std::abs(f_mid) < tol || (jg_hi - jg_lo) < tol) {
            return jg_mid;
        }

        if (f_lo * f_mid < 0.0) {
            jg_hi = jg_mid;
        } else {
            jg_lo = jg_mid;
            f_lo = f_mid;
        }
    }
    return jg_mid;
}

// -----------------------------
// メイン: jf = 0.01 ~ 10 [m/s] を走査
// -----------------------------
int main() {
    FluidParams p;
    p.rho_f = 1000.0;   // 水
    p.rho_g = 1.2;      // 空気
    p.nu_f  = 1.0e-6;   // 水の動粘度
    p.D     = 0.0276;   // 管径 [m]

    std::cout << "jf [m/s]\tjg_transition [m/s]\n";

    // jf = 0.01 ~ 10 を対数刻みで走査(例:30点)
    const int N = 30;
    double jf_min = 0.01;
    double jf_max = 10.0;
    double logMin = std::log10(jf_min);
    double logMax = std::log10(jf_max);

    std::ofstream csv("result.csv");
    for (int i = 0; i < N; ++i) {
        double jf = std::pow(10.0, logMin + i * (logMax - logMin) / (N - 1));

        try {
        double jg = solveJg(jf, p);
        csv << jf << "," << jg << ",OK\n";
    }
    catch (const std::exception& e) {
        csv << jf << ",,\"" << e.what() << "\"\n";
    }

    }
    csv.close();

    return 0;
}