#include <iostream>
#include <vector>
#include <chrono>

struct FluidParticles {
    int id;
    double x;
    double y;
    double temp;
};

// ==========================================
// パターン①：reserve【なし】バージョン（大引っ越し発生）
// ==========================================
std::vector<FluidParticles> generateParticles_NoReserve(int N) {
    std::vector<FluidParticles> particles;
    // ❌ わざと reserve を書かない！

    for(int i = 0; i < N; i++) {
        FluidParticles p;
        p.id = i;
        p.x = i * 0.5;
        p.y = i * 0.5;
        p.temp = 20.0;
        
        particles.push_back(p); // 忘れずに配列へ追加！
    }
    return particles; // 忘れずに返す！
}

// ==========================================
// パターン②：reserve【あり】バージョン（劇つよ）
// ==========================================
std::vector<FluidParticles> generateParticles_WithReserve(int N) {
    std::vector<FluidParticles> particles;
    
    // ✅ ループの前に1000万個分の土地をドン！と確保
    particles.reserve(N);

    for(int i = 0; i < N; i++) {
        FluidParticles p;
        p.id = i;
        p.x = i * 0.5;
        p.y = i * 0.5;
        p.temp = 20.0;
        
        particles.push_back(p);
    }
    return particles;
}

// ==========================================
// メイン関数：2つを比較する
// ==========================================
int main() {
    const int N = 10000000; // 1000万個

    std::cout << "initialization... \n\n";

    // --- ① reserve【なし】の計測 ---
    auto start1 = std::chrono::high_resolution_clock::now();
    std::vector<FluidParticles> particles_bad = generateParticles_NoReserve(N);
    auto end1 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed1 = end1 - start1;
    std::cout << "🐢 reserve無し : " << elapsed1.count() << " ms\n";

    // --- ② reserve【あり】の計測 ---
    auto start2 = std::chrono::high_resolution_clock::now();
    std::vector<FluidParticles> particles_good = generateParticles_WithReserve(N);
    auto end2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed2 = end2 - start2;
    std::cout << "🚀 reserve有り : " << elapsed2.count() << " ms\n";

    // --- 結果発表 ---
    std::cout << "\n🏆 結果: reserve有りは無しの " 
              << (elapsed1.count() / elapsed2.count()) 
              << " 倍速い！\n";

    return 0;
}