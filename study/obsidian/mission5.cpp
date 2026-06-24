#include <iostream>
#include <vector>
#include <chrono>
#include <memory>

struct RadiationPixel{
    int id;
    double intensity;
};


double calcAverageIntensity(const std::vector<RadiationPixel>& pixels){
    double sum = 0.0;
    for(int i=0; i<=pixels.size(); i++){
        sum += pixels[i].intensity;
    }
    return sum/pixels.size();
}

std::vector<RadiationPixel> convertToPixels(const std::unique_ptr<double[]>& raw_data, int N){
    
    std::vector<RadiationPixel> pixels;
    pixels.reserve(N);

    for(int i=0; i < N; i++){
        RadiationPixel p;
        p.id = i;
        p.intensity = raw_data[i];
        pixels.push_back(p);
    }

    return pixels;
}

int main(){
    const int N = 10000000;
    std::unique_ptr<double[]> raw_data = std::make_unique<double[]>(N);
    // センサーからのデータ受信（普通の配列と同じように使えます）
    for(int i = 0; i < N; i++) {
        raw_data[i] = 10.0 + (i % 5);
    }

    auto start = std::chrono::high_resolution_clock::now();

    // 変換と計算
    std::vector<RadiationPixel> image_data = convertToPixels(raw_data, N);
    double average = calcAverageIntensity(image_data);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;

    std::cout << "✅ 処理完了！\n";
    std::cout << "計算された平均放射線強度: " << average << "\n";
    std::cout << "⏱ 実行時間: " << elapsed.count() << " ms\n";
}