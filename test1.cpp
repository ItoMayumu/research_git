#include <iostream>
#include <vector>
#include <chrono>

// ==========================================
// ① 構造体の定義 (Mission 1.5)
// ここに RadiationPixel という名前の構造体を定義してください。
// 中身の変数は以下の2つです。
// - int id;
// - double intensity;
// ==========================================

/* 【あなたのコード①】 */
struct RadiationPixel{
    int id;
    double intensity;
};



// ==========================================
// ② 平均値計算関数 (Mission 2)
// 関数名: calcAverageIntensity
// 引数: RadiationPixel の vector を「定数参照渡し（コピーなし＆安全）」で受け取る
// 戻り値: double (平均強度)
// ==========================================
double calcAverageIntensity(const std::vector<RadiationPixel> pixels) {
    double sum = 0.0;
    
    // ループで全ピクセルの intensity を足し合わせる
    for(int i = 0; i < pixels.size(); i++) {
        sum += pixels[i].intensity;
    }
    
    return sum / pixels.size();
}


// ==========================================
// ③ データ変換関数 (Mission 1)
// 関数名: convertToPixels
// 引数: double* raw_data (生データ), int N (ピクセル数)
// 戻り値: std::vector<RadiationPixel>
// ==========================================
std::vector<RadiationPixel> convertToPixels(double* raw_data, int N) {
    std::vector<RadiationPixel> pixels;
    
    /* 【あなたのコード③】 N個分のメモリを予約(reserve)して大引っ越しを防ぐ */
    pixels.reserve(N);

    for(int i = 0; i < N; i++) {
        RadiationPixel p;
        p.id = i;
        p.intensity = raw_data[i];
        pixels.push_back(p);
    }
    
    return pixels;
}


// ==========================================
// メイン関数（パイプラインの実行）
// ==========================================
int main() {
    const int N = 10000000; // 1000万ピクセルの高解像度データ
    std::cout << "放射線画像データパイプラインを起動します...\n\n";

    // ----------------------------------------------------
    // ④ 生データのメモリ確保 (Mission 4 / Mission 3)
    // 巨大データなのでスタック領域は使えません。
    // new を使ってヒープ領域に N個の double 型配列を確保し、
    // ポインタ変数 raw_data で受け取ってください。
    // ----------------------------------------------------
    
    /* 【あなたのコード④】 */
    double* raw_data = new double[N];

    


    // (センサーからのデータ受信をシミュレート)
    for(int i = 0; i < N; i++) {
        raw_data[i] = 10.0 + (i % 5); // 10.0〜14.0のダミー放射線強度
    }

    // パイプライン処理の時間を計測開始
    auto start = std::chrono::high_resolution_clock::now();

    // Step 1: 生データから構造体ベクタへの変換
    std::vector<RadiationPixel> image_data = convertToPixels(raw_data, N);

    // ----------------------------------------------------
    // ⑤ 土地の返却 (Mission 4)
    // 変換が終わったので、用済みとなった raw_data のメモリを
    // delete[] を使って解放し、メモリリークを防いでください。
    // ----------------------------------------------------
    
    /* 【あなたのコード⑤】 */
    delete[] raw_data;
    


    // Step 2: 参照渡しによる爆速の平均値計算
    double average = calcAverageIntensity(image_data);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;

    // 結果出力
    std::cout << "✅ 処理完了！\n";
    std::cout << "計算された平均放射線強度: " << average << "\n";
    std::cout << "⏱ 実行時間: " << elapsed.count() << " ms\n";
    std::cout << "(メモリリークも大引っ越しも全コピーも発生していない完璧な挙動です)\n";

    return 0;
}