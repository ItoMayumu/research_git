#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>

const int NX = 41;           // x方向の格子数
const int NY = 41;           // y方向の格子数
const double L = 1.0;        // 領域の1辺の長さ
const double DX = L / (NX - 1);
const double DY = L / (NY - 1);
const double DT = 0.001;     // 時間刻み幅（コマ送りの間隔）
const double NU = 0.01;      // 動粘性係数（流体のドロドロ具合）
const double RHO = 1.0;      // 流体の密度
const int MAX_STEPS = 3000;  // 計算ステップ数

// 2次元(x,y)座標を1次元配列のインデックス番号に変換する関数
inline int idx(int x, int y) {
    return y * NX + x;
}

int main() {
    // 物理量の配列を確保 (すべて0.0で初期化)
    std::vector<double> u(NX * NY, 0.0);       // 本物のx方向速度
    std::vector<double> v(NX * NY, 0.0);       // 本物のy方向速度
    std::vector<double> p(NX * NY, 0.0);       // 圧力
    std::vector<double> u_star(NX * NY, 0.0);  // 仮のx方向速度
    std::vector<double> v_star(NX * NY, 0.0);  // 仮のy方向速度

    // --- メインの時間発展ループ (コマ送り) ---
    for (int step = 0; step <= MAX_STEPS; ++step) {

        // ==========================================
        // フェーズ2: 予測子ステップ (仮の速度 u*, v* を計算)
        // ==========================================
        // 圧力を一旦無視して、「流体の勢い(対流)」と「粘り(粘性)」だけで未来を予測する
        for (int y = 1; y < NY - 1; ++y) {
            for (int x = 1; x < NX - 1; ++x) {
                int c = idx(x, y);

                double u_c = u[c];
                double v_c = v[c];
                
                // (A) 対流項 (1次風上差分: 風上側のデータを使って計算の爆発を防ぐ)
                double dudx = (u_c > 0) ? (u_c - u[idx(x-1, y)]) / DX : (u[idx(x+1, y)] - u_c) / DX;
                double dudy = (v_c > 0) ? (u_c - u[idx(x, y-1)]) / DY : (u[idx(x, y+1)] - u_c) / DY;
                double adv_u = u_c * dudx + v_c * dudy;

                double dvdx = (u_c > 0) ? (v_c - v[idx(x-1, y)]) / DX : (v[idx(x+1, y)] - v_c) / DX;
                double dvdy = (v_c > 0) ? (v_c - v[idx(x, y-1)]) / DY : (v[idx(x, y+1)] - v_c) / DY;
                double adv_v = u_c * dvdx + v_c * dvdy;

                // (B) 粘性項 (2次中心差分: 周りの流体に引きずられる効果)
                double diff_u = NU * (
                    (u[idx(x+1, y)] - 2.0*u_c + u[idx(x-1, y)]) / (DX * DX) +
                    (u[idx(x, y+1)] - 2.0*u_c + u[idx(x, y-1)]) / (DY * DY)
                );
                double diff_v = NU * (
                    (v[idx(x+1, y)] - 2.0*v_c + v[idx(x-1, y)]) / (DX * DX) +
                    (v[idx(x, y+1)] - 2.0*v_c + v[idx(x, y-1)]) / (DY * DY)
                );

                // 仮の速度を更新
                u_star[c] = u_c + DT * (-adv_u + diff_u);
                v_star[c] = v_c + DT * (-adv_v + diff_v);
            }
        }

        // 仮の速度の境界条件 (上の壁だけが右に動く箱)
        for (int x = 0; x < NX; ++x) {
            u_star[idx(x, NY-1)] = 1.0; v_star[idx(x, NY-1)] = 0.0;
            u_star[idx(x, 0)]    = 0.0; v_star[idx(x, 0)]    = 0.0;
        }
        for (int y = 0; y < NY; ++y) {
            u_star[idx(0, y)]    = 0.0; v_star[idx(0, y)]    = 0.0;
            u_star[idx(NX-1, y)] = 0.0; v_star[idx(NX-1, y)] = 0.0;
        }

        // ==========================================
        // フェーズ3: 圧力ポアソン方程式 (SOR法)
        // ==========================================
        // 仮の速度が引き起こす「渋滞」を解消するような圧力の坂道(勾配)を計算する
        double omega = 1.7; // 加速係数 (収束を早める魔法の数字)
        for (int iter = 0; iter < 50; ++iter) { 
            for (int y = 1; y < NY - 1; ++y) {
                for (int x = 1; x < NX - 1; ++x) {
                    int c = idx(x, y);

                    // (E) 湧き出し量: 仮の速度のままだと、どれくらい流体が圧縮されてしまうか
                    double div_u_star = (u_star[idx(x+1, y)] - u_star[idx(x-1, y)]) / (2.0 * DX) +
                                        (v_star[idx(x, y+1)] - v_star[idx(x, y-1)]) / (2.0 * DY);

                    // (F) 渋滞解消に必要な新しい圧力を計算
                    double p_new = 0.25 * (
                        p[idx(x+1, y)] + p[idx(x-1, y)] +
                        p[idx(x, y+1)] + p[idx(x, y-1)] -
                        (RHO / DT) * div_u_star * DX * DY
                    );
                    
                    // (G) SOR法で思い切って値を更新
                    p[c] = (1.0 - omega) * p[c] + omega * p_new;
                }
            }
            
            // (H) 圧力の境界条件 (壁に向かって流体が押し込まれないように勾配をゼロにする)
            for (int x = 0; x < NX; ++x) {
                p[idx(x, 0)]    = p[idx(x, 1)];
                p[idx(x, NY-1)] = p[idx(x, NY-2)];
            }
            for (int y = 0; y < NY; ++y) {
                p[idx(0, y)]    = p[idx(1, y)];
                p[idx(NX-1, y)] = p[idx(NX-2, y)];
            }
        }

        // ==========================================
        // フェーズ4: 修正子ステップ (速度の確定と結果出力)
        // ==========================================
        // (I) 確定した圧力の坂道を使って、仮の速度を正しい速度に修正する
        for (int y = 1; y < NY - 1; ++y) {
            for (int x = 1; x < NX - 1; ++x) {
                int c = idx(x, y);
                u[c] = u_star[c] - (DT / RHO) * (p[idx(x+1, y)] - p[idx(x-1, y)]) / (2.0 * DX);
                v[c] = v_star[c] - (DT / RHO) * (p[idx(x, y+1)] - p[idx(x, y-1)]) / (2.0 * DY);
            }
        }

        // 最終的な速度の境界条件を再設定
        for (int x = 0; x < NX; ++x) {
            u[idx(x, NY-1)] = 1.0; v[idx(x, NY-1)] = 0.0;
            u[idx(x, 0)]    = 0.0; v[idx(x, 0)]    = 0.0;
        }
        for (int y = 0; y < NY; ++y) {
            u[idx(0, y)]    = 0.0; v[idx(0, y)]    = 0.0;
            u[idx(NX-1, y)] = 0.0; v[idx(NX-1, y)] = 0.0;
        }

        // 進捗の表示
        if (step % 500 == 0) {
            std::cout << "Step: " << step << " / " << MAX_STEPS << std::endl;
        }
    }

    // (L) CSV形式でデータを保存
    std::ofstream file("./data/result.csv");
    file << "x,y,u,v,p\n";
    for (int y = 0; y < NY; ++y) {
        for (int x = 0; x < NX; ++x) {
            file << x * DX << "," << y * DY << "," 
                 << u[idx(x, y)] << "," << v[idx(x, y)] << "," << p[idx(x, y)] << "\n";
        }
    }
    file.close();
    std::cout << "Simulation completed. Data saved to result.csv." << std::endl;

    return 0;
}