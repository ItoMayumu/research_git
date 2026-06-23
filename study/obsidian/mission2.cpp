#include <iostream>
#include <vector>
#include <chrono>

struct FluidParticle {
    int id;
    double x;
    double y;
    double temp;
};

std::vector<FluidParticle> generateParticles(int N) {
    std::vector<FluidParticle> particles;
    particles.reserve(N);
    for(int i = 0; i < N; i++) {
        FluidParticle p;
        p.id = i;
        p.x = i * 0.5;
        p.y = i * 0.5;
        p.temp = 20.0 + (i % 10); 
        particles.push_back(p);
    }
    return particles;
}

double calcAverageTempBad(std::vector<FluidParticle> particles) {
    double sum = 0.0;
    for(int i=0; i< particles.size(); i++){
        sum += particles[i].temp;
    }
    double ave = sum / 10000000;

    return ave;

}
double calcAverageTempGood(const std::vector<FluidParticle>& particles) {
    double sum = 0.0;
    
    for(int i = 0; i < particles.size(); i++){
        sum += particles[i].temp;
    }
    
    double ave = sum / particles.size();
    return ave;
}



int main() {
    const int N = 10000000; // 1000万個
    std::cout << "データの準備中...\n";
    std::vector<FluidParticle> my_particles = generateParticles(N);
    std::cout << "準備完了！平均温度の計算テストを開始します。\n\n";

    auto start1 = std::chrono::high_resolution_clock::now();
    double avg1 = calcAverageTempBad(my_particles);
    auto end1 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed1 = end1 - start1;
    std::cout << "🐢 値渡し (Bad) : " << elapsed1.count() << " ms (平均温度: " << avg1 << ")\n";

    auto start2 = std::chrono::high_resolution_clock::now();
    double avg2 = calcAverageTempGood(my_particles);
    auto end2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed2 = end2 - start2;
    std::cout << "🚀 参照渡し(Good): " << elapsed2.count() << " ms (平均温度: " << avg2 << ")\n";

    std::cout << "\n🏆 結果: 参照渡しは値渡しの " 
              << (elapsed1.count() / elapsed2.count()) 
              << " 倍速い！\n";

    return 0;
}