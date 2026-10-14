// cpp04_ode.cpp  (目標: 30分)
//
// 身につくスキル:
//   状態ベクトル型(std::vector<double>) / 汎用の1ステップ関数 /
//   オイラー法・RK4 / 誤差と収束次数 / エネルギー保存の比較 /
//   CSVファイル出力(ofstream)
//
// build: g++ -std=c++17 -O2 -Wall -Wextra cpp04_ode.cpp -o cpp04 && ./cpp04
// 出力: ode_lv.csv (Lotka-Volterra), pythonで pandas.read_csv して plot してみること
#include <cassert>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <functional>
#include <vector>

using State = std::vector<double>;
using RHS = std::function<State(double, const State&)>;

State axpy(const State& y, double a, const State& k) {  // y + a*k
    State r(y.size());
    for (size_t i = 0; i < y.size(); ++i) r[i] = y[i] + a * k[i];
    return r;
}

State euler_step(const RHS& f, double t, const State& y, double h) {
    return axpy(y, h, f(t, y));
}

State rk4_step(const RHS& f, double t, const State& y, double h) {
    State k1 = f(t, y);
    State k2 = f(t + h / 2, axpy(y, h / 2, k1));
    State k3 = f(t + h / 2, axpy(y, h / 2, k2));
    State k4 = f(t + h, axpy(y, h, k3));
    State r(y.size());
    for (size_t i = 0; i < y.size(); ++i)
        r[i] = y[i] + h / 6 * (k1[i] + 2 * k2[i] + 2 * k3[i] + k4[i]);
    return r;
}

using Stepper = std::function<State(const RHS&, double, const State&, double)>;

State integrate(const Stepper& step, const RHS& f, double t0, double t1, State y, int n) {
    double h = (t1 - t0) / n;
    for (int i = 0; i < n; ++i) y = step(f, t0 + i * h, y, h);
    return y;
}

int main() {
    const double PI = std::acos(-1.0);

    // --- 1. 調和振動子 y''=-y, y(0)=1, y'(0)=0 → y(2π)=1 ---
    RHS sho = [](double, const State& y) { return State{y[1], -y[0]}; };
    std::printf("%6s %12s %12s\n", "n", "err_euler", "err_rk4");
    double pe = 0, pr = 0, ord_e = 0, ord_r = 0;
    for (int n = 50; n <= 800; n *= 2) {
        // 2π後の厳密解は (1, 0)。状態ベクトル全体のノルムで誤差を測る
        State se = integrate(euler_step, sho, 0, 2 * PI, {1, 0}, n);
        State sr = integrate(rk4_step, sho, 0, 2 * PI, {1, 0}, n);
        double ee = std::hypot(se[0] - 1.0, se[1]);
        double er = std::hypot(sr[0] - 1.0, sr[1]);
        std::printf("%6d %12.3e %12.3e\n", n, ee, er);
        if (n > 50) { ord_e = std::log2(pe / ee); ord_r = std::log2(pr / er); }
        pe = ee; pr = er;
    }
    std::printf("order: euler=%.2f rk4=%.2f\n", ord_e, ord_r);
    assert(std::abs(ord_e - 1) < 0.1 && std::abs(ord_r - 4) < 0.2);

    // --- 2. エネルギー保存: 長時間 (t=0..100) ---
    int n = 2000;
    State ye = integrate(euler_step, sho, 0, 100, {1, 0}, n);
    State yr = integrate(rk4_step, sho, 0, 100, {1, 0}, n);
    auto energy = [](const State& y) { return 0.5 * (y[0] * y[0] + y[1] * y[1]); };
    std::printf("energy (exact 0.5): euler=%.4f rk4=%.8f\n", energy(ye), energy(yr));

    // --- 3. Lotka-Volterra → CSV ---
    const double a = 1.0, b = 0.1, c = 1.5, d = 0.075;
    RHS lv = [=](double, const State& y) {
        return State{a * y[0] - b * y[0] * y[1], -c * y[1] + d * y[0] * y[1]};
    };
    // 保存量 H = d*x - c*ln x + b*y - a*ln y
    auto H = [=](const State& y) { return d * y[0] - c * std::log(y[0]) + b * y[1] - a * std::log(y[1]); };
    std::ofstream ofs("ode_lv.csv");
    ofs << "t,prey,predator,H\n";
    State y = {10.0, 5.0};
    double h = 0.01, t = 0.0, H0 = H(y), drift = 0.0;
    for (int i = 0; i <= 5000; ++i) {
        if (i % 10 == 0) ofs << t << ',' << y[0] << ',' << y[1] << ',' << H(y) << '\n';
        drift = std::max(drift, std::abs(H(y) - H0));
        y = rk4_step(lv, t, y, h);
        t += h;
    }
    std::printf("Lotka-Volterra: max |H-H0| = %.3e (saved ode_lv.csv)\n", drift);
    assert(drift < 1e-6);

    std::puts("cpp04 OK");
    return 0;
}
