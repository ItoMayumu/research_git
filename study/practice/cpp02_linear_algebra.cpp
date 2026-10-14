// cpp02_linear_algebra.cpp  (目標: 30分)
//
// 身につくスキル:
//   Matrixクラス(1次元配列 + operator()) / 行列ベクトル積 /
//   LU分解(部分ピボット) / 前進・後退代入 / 行列式 / 共役勾配法(CG) /
//   残差による検証
//
// build: g++ -std=c++17 -O2 -Wall -Wextra cpp02_linear_algebra.cpp -o cpp02 && ./cpp02
#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <utility>
#include <vector>

using Vec = std::vector<double>;

class Matrix {
public:
    Matrix(int r, int c, double v = 0.0) : r_(r), c_(c), a_(static_cast<size_t>(r) * c, v) {}
    double& operator()(int i, int j) { return a_[static_cast<size_t>(i) * c_ + j]; }
    double operator()(int i, int j) const { return a_[static_cast<size_t>(i) * c_ + j]; }
    int rows() const { return r_; }
    int cols() const { return c_; }

    Vec mul(const Vec& x) const {
        Vec y(r_, 0.0);
        for (int i = 0; i < r_; ++i)
            for (int j = 0; j < c_; ++j) y[i] += (*this)(i, j) * x[j];
        return y;
    }

private:
    int r_, c_;
    Vec a_;
};

double dot(const Vec& a, const Vec& b) { return std::inner_product(a.begin(), a.end(), b.begin(), 0.0); }
double norm(const Vec& a) { return std::sqrt(dot(a, a)); }

// ---------- LU分解 (PA = LU, in-place) ----------
struct LU {
    Matrix lu;
    std::vector<int> piv;
    int sign = 1;
    explicit LU(Matrix A) : lu(std::move(A)), piv(lu.rows()) {
        const int n = lu.rows();
        std::iota(piv.begin(), piv.end(), 0);
        for (int k = 0; k < n; ++k) {
            int p = k;
            for (int i = k + 1; i < n; ++i)
                if (std::abs(lu(i, k)) > std::abs(lu(p, k))) p = i;
            if (std::abs(lu(p, k)) < 1e-14) throw std::runtime_error("singular matrix");
            if (p != k) {
                for (int j = 0; j < n; ++j) std::swap(lu(k, j), lu(p, j));
                std::swap(piv[k], piv[p]);
                sign = -sign;
            }
            for (int i = k + 1; i < n; ++i) {
                lu(i, k) /= lu(k, k);
                for (int j = k + 1; j < n; ++j) lu(i, j) -= lu(i, k) * lu(k, j);
            }
        }
    }
    Vec solve(const Vec& b) const {
        const int n = lu.rows();
        Vec x(n);
        for (int i = 0; i < n; ++i) x[i] = b[piv[i]];       // 並べ替え
        for (int i = 0; i < n; ++i)                         // L y = Pb
            for (int j = 0; j < i; ++j) x[i] -= lu(i, j) * x[j];
        for (int i = n - 1; i >= 0; --i) {                  // U x = y
            for (int j = i + 1; j < n; ++j) x[i] -= lu(i, j) * x[j];
            x[i] /= lu(i, i);
        }
        return x;
    }
    double det() const {
        double d = sign;
        for (int i = 0; i < lu.rows(); ++i) d *= lu(i, i);
        return d;
    }
};

// ---------- 共役勾配法 (A は対称正定値) ----------
int conjugate_gradient(const Matrix& A, const Vec& b, Vec& x, double tol = 1e-10, int maxit = 1000) {
    const int n = A.rows();
    x.assign(n, 0.0);
    Vec r = b, p = r;
    double rs = dot(r, r);
    for (int k = 0; k < maxit; ++k) {
        Vec Ap = A.mul(p);
        double alpha = rs / dot(p, Ap);
        for (int i = 0; i < n; ++i) { x[i] += alpha * p[i]; r[i] -= alpha * Ap[i]; }
        double rs_new = dot(r, r);
        if (std::sqrt(rs_new) < tol * norm(b)) return k + 1;
        for (int i = 0; i < n; ++i) p[i] = r[i] + (rs_new / rs) * p[i];
        rs = rs_new;
    }
    return maxit;
}

int main() {
    // 小さい例: 手計算できる
    Matrix A(3, 3);
    double vals[3][3] = {{2, 1, 1}, {4, 3, 3}, {8, 7, 9}};
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j) A(i, j) = vals[i][j];
    LU lu(A);
    Vec b = {4, 10, 24};
    Vec x = lu.solve(b);
    Vec Ax = A.mul(x);
    double res = 0;
    for (int i = 0; i < 3; ++i) res = std::max(res, std::abs(Ax[i] - b[i]));
    std::cout << "x = " << x[0] << ", " << x[1] << ", " << x[2] << "  residual=" << res << "\n";
    std::cout << "det = " << lu.det() << " (expect 4)\n";
    assert(res < 1e-12 && std::abs(lu.det() - 4.0) < 1e-12);

    // 1D Poisson 三重対角 (SPD) を LU と CG で解いて比較
    const int n = 100;
    Matrix T(n, n);
    for (int i = 0; i < n; ++i) {
        T(i, i) = 2.0;
        if (i > 0) T(i, i - 1) = -1.0;
        if (i < n - 1) T(i, i + 1) = -1.0;
    }
    Vec rhs(n, 1.0);
    Vec x_lu = LU(T).solve(rhs);
    Vec x_cg;
    int iters = conjugate_gradient(T, rhs, x_cg);
    double diff = 0;
    for (int i = 0; i < n; ++i) diff = std::max(diff, std::abs(x_lu[i] - x_cg[i]));
    std::cout << "CG iterations = " << iters << ", max|x_lu - x_cg| = " << diff << "\n";
    assert(diff < 1e-6);

    std::cout << "cpp02 OK\n";
    return 0;
}
