#include <iostream>

void processImageFrame(int frame_number){
    const int N = 10000000;

    double* image_data = new double[N];
    image_data[0] = 1.0;
    image_data[N - 1] = 0.5;

    // ==========================================
    // 【あなたのコード②】土地を確実に返却する
    // ==========================================
    delete[] image_data;
}

int main() {
    std::cout << "放射線画像の連続フレーム処理を開始します...\n";
    
    // 50枚の画像を連続で処理する（80MB × 50回 = 累計4GBのメモリ操作）
    for(int i = 0; i < 50; i++) {
        processImageFrame(i);
        
        if (i % 10 == 0) {
            std::cout << i << " フレーム目の処理が完了しました。\n";
        }
    }
    
    std::cout << "無事、すべての処理が完了し、メモリも健全です！\n";
    return 0;
}