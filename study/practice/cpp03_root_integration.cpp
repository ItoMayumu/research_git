// cpp03_root_integration.cpp  (目標: 30分)
//
// 身につくスキル:
//   std::function で関数を渡す / 二分法・ニュートン法・割線法 /
//   台形・シンプソン・ガウス=ルジャンドル(3点) / 収束次数の数値的確認 /
//   printfによる表出力
//
// build: g++ -std=c++17 -O2 -Wall -Wextra cpp03_root_integration.cpp -o cpp03 && ./cpp03
#include <cassert>
#include <cmath>
#include <cstdio>
#include <functional>
#include <stdexcept>

using Func = std::function<double(double)>;
const double PI = std::acos(-1.0);

// ---------- 求根 ----------
double bisection(const Func& f, double a, double b, double tol = 1e-12) {
    double fa = f(a), fb = f(b);
    if (fa * fb > 0) throw std::invalid_argument("no sign change");
    while (b - a > tol) {
        double c = 0.5 * (a + b), fc = f(c);
        if (fa * fc <= 0) { b = c; fb = fc; } else { a = c; fa = fc; }
    }
    return 0.5 * (a + b);
}

double newton(const Func& f, const Func& df, double x, int& iters, double tol = 1e-13) {
    for (iters = 1; iters <= 100; ++iters) {
        double dx = f(x) / df(x);
        x -= dx;
        if (std::abs(dx) < tol) return x;
    }
    throw std::runtime_error("newton: not converged");
}

double secant(const Func& f, double x0, double x1, int& iters, double tol = 1e-13) {
    double f0 = f(x0), f1 = f(x1);
    for (iters = 1; iters <= 100; ++iters) {
        double x2 = x1 - f1 * (x1 - x0) / (f1 - f0);
        x0 = x1; f0 = f1;
        x1 = x2; f1 = f(x1);
        if (std::abs(x1 - x0) < tol) return x1;
    }
    throw std::runtime_error("secant: not converged");
}

// ---------- 積分 ----------
double trapezoid(const Func& f, double a, double b, int n) {
    double h = (b - a) / n, s = 0.5 * (f(a) + f(b));
    for (int i = 1; i < n; ++i) s += f(a + i * h);
    return s * h;
}

double simpson(const Func& f, double a, double b, int n) {  // n は偶数
    double h = (b - a) / n, s = f(a) + f(b);
    for (int i = 1; i < n; ++i) s += (i % 2 ? 4.0 : 2.0) * f(a + i * h);
    return s * h / 3.0;
}

// 区間をn分割し、各小区間で3点ガウス=ルジャンドル
double gauss3(const Func& f, double a, double b, int n) {
    const double xi[3] = {-std::sqrt(0.6), 0.0, std::sqrt(0.6)};
    const double wi[3] = {5.0 / 9.0, 8.0 / 9.0, 5.0 / 9.0};
    double h = (b - a) / n, s = 0.0;
    for (int i = 0; i < n; ++i) {
        double mid = a + (i + 0.5) * h;
        for (int k = 0; k < 3; ++k) s += wi[k] * f(mid + 0.5 * h * xi[k]);
    }
    return s * 0.5 * h;
}

int main() {
    // --- 求根: cos(x) = x ---
    Func f = [](double x) { return std::cos(x) - x; };
    Func df = [](double x) { return -std::sin(x) - 1.0; };
    int it_n = 0, it_s = 0;
    double r_b = bisection(f, 0.0, 1.0);
    double r_n = newton(f, df, 0.5, it_n);
    double r_s = secant(f, 0.0, 1.0, it_s);
    std::printf("bisection %.14f\nnewton    %.14f (%d iter)\nsecant    %.14f (%d iter)\n",
                r_b, r_n, it_n, r_s, it_s);
    assert(std::abs(r_b - r_n) < 1e-10 && std::abs(r_n - r_s) < 1e-10);

    // --- 積分: ∫0^pi sin x dx = 2 ---
    Func g = [](double x) { return std::sin(x); };
    std::printf("\n%6s %12s %12s %12s\n", "n", "trap", "simpson", "gauss3");
    double prev_t = 0, prev_s = 0, prev_g = 0, ord_t = 0, ord_s = 0, ord_g = 0;
    for (int n = 4; n <= 64; n *= 2) {
        double et = std::abs(trapezoid(g, 0, PI, n) - 2.0);
        double es = std::abs(simpson(g, 0, PI, n) - 2.0);
        double eg = std::abs(gauss3(g, 0, PI, n) - 2.0);
        std::printf("%6d %12.3e %12.3e %12.3e\n", n, et, es, eg);
        if (n > 4) {  // 刻みを半分にしたときの誤差比から次数を推定
            ord_t = std::log2(prev_t / et);
            ord_s = std::log2(prev_s / es);
            ord_g = std::log2(prev_g / eg);
        }
        prev_t = et; prev_s = es; prev_g = eg;
    }
    std::printf("estimated order: trap=%.2f simpson=%.2f gauss3=%.2f\n", ord_t, ord_s, ord_g);
    assert(std::abs(ord_t - 2) < 0.1 && std::abs(ord_s - 4) < 0.2 && ord_g > 5.5);

    std::puts("cpp03 OK");
    return 0;
}
