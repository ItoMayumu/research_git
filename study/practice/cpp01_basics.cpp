// cpp01_basics.cpp  (目標: 30分)
//
// 身につくスキル:
//   テンプレート関数 / クラスと演算子オーバーロード / vector・map /
//   STLアルゴリズム(sort, accumulate, transform) / ラムダ / std::function /
//   例外 / iomanipによる出力整形 / 参照とconst
//
// build: g++ -std=c++17 -O2 -Wall -Wextra cpp01_basics.cpp -o cpp01 && ./cpp01
#include <algorithm>
#include <cassert>
#include <cmath>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <numeric>
#include <stdexcept>
#include <string>
#include <vector>

// ---------- テンプレート ----------
template <typename T>
T mean(const std::vector<T>& v) {
    if (v.empty()) throw std::invalid_argument("mean: empty vector");
    return std::accumulate(v.begin(), v.end(), T{}) / static_cast<T>(v.size());
}

template <typename T>
T variance(const std::vector<T>& v) {
    T m = mean(v);
    T s = T{};
    for (const T& x : v) s += (x - m) * (x - m);
    return s / static_cast<T>(v.size() - 1);
}

// ---------- 3次元ベクトルクラス ----------
class Vec3 {
public:
    double x, y, z;
    Vec3(double x_ = 0, double y_ = 0, double z_ = 0) : x(x_), y(y_), z(z_) {}

    Vec3 operator+(const Vec3& o) const { return {x + o.x, y + o.y, z + o.z}; }
    Vec3 operator-(const Vec3& o) const { return {x - o.x, y - o.y, z - o.z}; }
    Vec3 operator*(double s) const { return {x * s, y * s, z * s}; }
    Vec3& operator+=(const Vec3& o) { x += o.x; y += o.y; z += o.z; return *this; }

    double dot(const Vec3& o) const { return x * o.x + y * o.y + z * o.z; }
    Vec3 cross(const Vec3& o) const {
        return {y * o.z - z * o.y, z * o.x - x * o.z, x * o.y - y * o.x};
    }
    double norm() const { return std::sqrt(dot(*this)); }
};
Vec3 operator*(double s, const Vec3& v) { return v * s; }
std::ostream& operator<<(std::ostream& os, const Vec3& v) {
    return os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
}

// ---------- 高階関数 ----------
double apply_twice(const std::function<double(double)>& f, double x) { return f(f(x)); }

int main() {
    // vector と STL
    std::vector<double> v = {3.5, 1.2, 4.8, 2.2, 5.0};
    std::cout << std::fixed << std::setprecision(3);
    std::cout << "mean=" << mean(v) << " var=" << variance(v) << "\n";

    std::sort(v.begin(), v.end(), [](double a, double b) { return a > b; });  // 降順
    for (double x : v) std::cout << x << ' ';
    std::cout << "\n";

    std::vector<double> sq(v.size());
    std::transform(v.begin(), v.end(), sq.begin(), [](double x) { return x * x; });
    auto [mn, mx] = std::minmax_element(sq.begin(), sq.end());
    std::cout << "min sq=" << *mn << " max sq=" << *mx << "\n";

    std::map<std::string, int> cnt;
    for (std::string w : {"a", "b", "a", "c", "a", "b"}) ++cnt[w];
    for (const auto& [k, c] : cnt) std::cout << k << ":" << c << " ";
    std::cout << "\n";
    assert(cnt["a"] == 3);

    // Vec3
    Vec3 a(1, 0, 0), b(0, 1, 0);
    Vec3 c = a.cross(b);
    std::cout << "a x b = " << c << " |c|=" << c.norm() << "\n";
    assert(std::abs(a.dot(b)) < 1e-15 && std::abs(c.z - 1) < 1e-15);
    Vec3 s = 2.0 * a + b * 3.0;
    s += Vec3(0, 0, 1);
    std::cout << "s = " << s << "\n";

    // std::function / ラムダのキャプチャ
    double k = 3.0;
    std::cout << "apply_twice(x*k)(2) = " << apply_twice([k](double x) { return x * k; }, 2.0) << "\n";

    // 例外
    try {
        std::vector<double> empty;
        mean(empty);
    } catch (const std::invalid_argument& e) {
        std::cout << "caught: " << e.what() << "\n";
    }

    std::cout << "cpp01 OK\n";
    return 0;
}
