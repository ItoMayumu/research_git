#include <iostream>
#include <vector>
#include <chrono>

int main() {
    const int W = 10000; // 画像の幅 (Width)
    const int H = 10000; // 画像の高さ (Height)
    
    std::cout << "1億ピクセルの巨大画像メモリを準備中...\n";
    // 1次元配列で2次元データ（1億ピクセル）を表現
    std::vector<double> image(W * H, 1.0); 
    std::cout << "準備完了！ キャッシュミスの実験を開始します。\n\n";

    // ==========================================
    // ❌ 悪いループ（列優先：Column-Major）
    // 縦方向に進むため、CPUキャッシュが効かず毎回遠くのメモリを読む
    // ==========================================
    auto start_bad = std::chrono::high_resolution_clock::now();

    for(int x = 0; x < W; x++) {
        for(int y = 0; y < H; y++) {
            image[y * W + x] += 0.1; 
        }
    }

    auto end_bad = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed_bad = end_bad - start_bad;
    std::cout << "🐢 悪いループ（縦に進む） : " << elapsed_bad.count() << " ms\n";

    // ==========================================
    // ✅ 良いループ（行優先：Row-Major）
    // メモリの並び通り（横方向）に進むため、CPUが隣のデータを爆速で読み込める
    // ==========================================
    auto start_good = std::chrono::high_resolution_clock::now();

    for(int y = 0; y < H; y++) {
        for(int x = 0; x < W; x++) {
            image[y * W + x] += 0.1; 
        }
    }

    auto end_good = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed_good = end_good - start_good;
    std::cout << "🚀 良いループ（横に進む） : " << elapsed_good.count() << " ms\n";

    // --- 結果発表 ---
    std::cout << "\n🏆 結果: 良いループは悪いループの " 
              << (elapsed_bad.count() / elapsed_good.count()) 
              << " 倍速い！\n";

    return 0;
}