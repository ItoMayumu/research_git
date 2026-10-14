// cpp05_pde.cpp  (目標: 30分)
//
// 身につくスキル:
//   1次元熱方程式 u_t = u_xx の差分法 (陽解法FTCS / Crank-Nicolson) /
//   安定性条件 r = dt/dx^2 <= 1/2 / Thomasアルゴリズム(三重対角) /
//   2次元Laplace方程式の Gauss-Seidel と SOR / 解析解との誤差評価
//
// build: g++ -std=c++17 -O2 -Wall -Wextra cpp05_pde.cpp -o cpp05 && ./cpp05
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <vector>

using Vec = std::vector<double>;
const double PI = std::acos(-1.0);

// 三重対角: a[i] x[i-1] + b[i] x[i] + c[i] x[i+1] = d[i]
Vec thomas(Vec a, Vec b, Vec c, Vec d) {
    const int n = static_cast<int>(b.size());
    for (int i = 1; i < n; ++i) {
        double w = a[i] / b[i - 1];
        b[i] -= w * c[i - 1];
        d[i] -= w * d[i - 1];
    }
    Vec x(n);
    x[n - 1] = d[n - 1] / b[n - 1];
    for (int i = n - 2; i >= 0; --i) x[i] = (d[i] - c[i] * x[i + 1]) / b[i];
    return x;
}

// u(x,0)=sin(pi x), u(0,t)=u(1,t)=0  →  厳密解 exp(-pi^2 t) sin(pi x)
double heat_explicit(int nx, double dt, double T) {
    double dx = 1.0 / nx, r = dt / (dx * dx);
    Vec u(nx + 1), un(nx + 1);
    for (int i = 0; i <= nx; ++i) u[i] = std::sin(PI * i * dx);
    int nt = static_cast<int>(std::lround(T / dt));
    for (int n = 0; n < nt; ++n) {
        for (int i = 1; i < nx; ++i) un[i] = u[i] + r * (u[i - 1] - 2 * u[i] + u[i + 1]);
        un[0] = un[nx] = 0.0;
        u.swap(un);
    }
    double err = 0;
    for (int i = 0; i <= nx; ++i)
        err = std::max(err, std::abs(u[i] - std::exp(-PI * PI * T) * std::sin(PI * i * dx)));
    return err;
}

double heat_cn(int nx, double dt, double T) {
    double dx = 1.0 / nx, r = dt / (dx * dx);
    const int m = nx - 1;  // 内点数
    Vec u(nx + 1);
    for (int i = 0; i <= nx; ++i) u[i] = std::sin(PI * i * dx);
    Vec a(m, -r / 2), b(m, 1 + r), c(m, -r / 2), d(m);
    int nt = static_cast<int>(std::lround(T / dt));
    for (int n = 0; n < nt; ++n) {
        for (int i = 1; i < nx; ++i)
            d[i - 1] = (1 - r) * u[i] + 0.5 * r * (u[i - 1] + u[i + 1]);
        Vec x = thomas(a, b, c, d);
        for (int i = 1; i < nx; ++i) u[i] = x[i - 1];
    }
    double err = 0;
    for (int i = 0; i <= nx; ++i)
        err = std::max(err, std::abs(u[i] - std::exp(-PI * PI * T) * std::sin(PI * i * dx)));
    return err;
}

// 2D Laplace: 単位正方形、上辺 u=sin(pi x)、他辺 0。 厳密解 sin(pi x) sinh(pi y)/sinh(pi)
// omega=1 で Gauss-Seidel、1<omega<2 で SOR。反復回数を返す
int laplace_sor(int n, double omega, double& max_err) {
    const double h = 1.0 / n;
    std::vector<Vec> u(n + 1, Vec(n + 1, 0.0));  // u[i][j] : x=i*h, y=j*h
    for (int i = 0; i <= n; ++i) u[i][n] = std::sin(PI * i * h);
    int it = 0;
    for (; it < 20000; ++it) {
        double diff = 0.0;
        for (int i = 1; i < n; ++i)
            for (int j = 1; j < n; ++j) {
                double gs = 0.25 * (u[i - 1][j] + u[i + 1][j] + u[i][j - 1] + u[i][j + 1]);
                double nv = (1 - omega) * u[i][j] + omega * gs;
                diff = std::max(diff, std::abs(nv - u[i][j]));
                u[i][j] = nv;
            }
        if (diff < 1e-10) break;
    }
    max_err = 0.0;
    for (int i = 0; i <= n; ++i)
        for (int j = 0; j <= n; ++j) {
            double ex = std::sin(PI * i * h) * std::sinh(PI * j * h) / std::sinh(PI);
            max_err = std::max(max_err, std::abs(u[i][j] - ex));
        }
    return it + 1;
}

int main() {
    // --- 熱方程式 ---
    const double T = 0.2;
    double e1 = heat_explicit(20, 0.4 / 400, T);   // r = 0.4 (安定)
    double e2 = heat_explicit(20, 0.6 / 400, T);   // r = 0.6 (不安定 → 誤差が爆発)
    double e3 = heat_cn(20, 0.005, T);             // r = 2.0 でも安定
    std::printf("explicit r=0.4 : err=%.3e\n", e1);
    std::printf("explicit r=0.6 : err=%.3e (unstable)\n", e2);
    std::printf("CN       r=2.0 : err=%.3e\n", e3);
    assert(e1 < 2e-3 && e2 > 1e-2 && e3 < 2e-3);

    // CN の時間2次精度: dt を半分にすると誤差が約1/4
    // 空間誤差を無視できるよう nx を十分大きく取る
    double c1 = heat_cn(1000, 0.004, T), c2 = heat_cn(1000, 0.002, T);
    double ord = std::log2(c1 / c2);
    std::printf("CN dt-order ~ %.2f\n", ord);
    assert(std::abs(ord - 2.0) < 0.2);

    // --- Laplace ---
    double err_gs, err_sor;
    int it_gs = laplace_sor(32, 1.0, err_gs);
    int it_sor = laplace_sor(32, 1.8, err_sor);
    std::printf("Gauss-Seidel: %d iters (err %.2e)\n", it_gs, err_gs);
    std::printf("SOR w=1.8   : %d iters (err %.2e)\n", it_sor, err_sor);
    assert(it_sor < it_gs && err_sor < 5e-3);

    std::puts("cpp05 OK");
    return 0;
}
