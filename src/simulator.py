import numpy as np
import matplotlib.pyplot as plt

# ==========================================
# 1. パラメータ設定
# ==========================================
# 【シミュレーション時間・歩進設定】
dt = 0.1          # 計算間隔 [s]
t_end = 250.0     # シミュレーション時間 [s]
time_steps = int(t_end / dt)

# 【環境（空気）の物性値 (at 40°C 想定)】
Ta = 25.0          # 外気温度 [°C]
rho = 1.127        # 密度 [kg/m^3]
mu = 1.918e-5      # 粘度 [Pa·s]
k_air = 0.0271     # 空気の熱伝導率 [W/m·K]
Pr = 0.706         # プラントル数

# 【ヒートシンクの幾何学的寸法・材料パラメータ (アルミ製)】
k_fin = 200.0      # アルミニウムの熱伝導率 [W/m·K]
fin_L = 0.150      # フィンの長さ [m] (150mm)
fin_H = 0.040      # フィンの高さ [m] (40mm)
fin_W = 0.120      # ヒートシンク全体の幅 [m] (120mm)
fin_t = 0.0015     # フィン1枚の厚み [m] (1.5mm)
fin_N = 35         # フィンの枚数 (35枚)

# 【ファンの仕様】
max_fan_flow = 0.035 # ファン100%時の最大風量 [m^3/s] (約74 CFM)

# 【各熱ノードの熱容量 C [J/K]】
C_chip = 2.5       # GPUコア
C_vram = 1.2       # VRAM
C_vrm  = 0.8       # VRM
C_sink = 50.0      # ヒートシンク構造体

# 【各内部コンポーネントからシンクベースまでの固体の熱抵抗 R [K/W]】
R_chip_s = 0.3     # GPUコア ➔ シンク（高性能グリス）
R_vram_s = 1.5     # VRAM ➔ シンク（サーマルパッド）
R_vrm_s  = 2.0     # VRM ➔ シンク（厚手のサーマルパッド）

# 【制御（PI制御）パラメータ】
T_target = 70.0    # 目標GPUコア温度 [°C]
Kp = 5.0           # 比例ゲイン
Ki = 0.03          # 積分ゲイン

# ==========================================
# 2. 物理・流体力学計算関数の定義
# ==========================================
def calculate_r_sa(pwm):
    """
    流体力学と伝熱工学の無次元数からヒートシンクの対外気熱抵抗 R_sa を動的計算する
    """
    # 1. 幾何学的な寸法計算
    s = (fin_W - (fin_N * fin_t)) / (fin_N - 1) # フィン隙間
    A_flow = (fin_N - 1) * s * fin_H             # 流路断面積
    A_total = (2.0 * fin_N * fin_L * fin_H) + ((fin_N - 1) * s * fin_L) # 全表面積
    
    # 2. 風量と風速の計算
    Q = max_fan_flow * pwm if pwm > 0.05 else 0.0001
    U = Q / A_flow  # フィン間風速 [m/s]
    
    # 3. 流体力学・伝熱特性の計算（無次元数）
    D_h = (2.0 * s * fin_H) / (s + fin_H) # 水力直径
    Re = (rho * U * D_h) / mu            # レイノルズ数
    
    if Re > 0.1:
        Nu = 0.664 * (Re**0.5) * (Pr**(1.0/3.0)) # 層流熱伝達実験式
    else:
        Nu = 3.66 # 自然対流/分子拡散の限界値
        
    h = (Nu * k_air) / D_h  # 熱伝達率 [W/m^2·K]
    
    # 4. フィン効率の計算 (先端での温度降下補正)
    m = np.sqrt((2.0 * h) / (k_fin * fin_t))
    fin_efficiency = np.tanh(m * fin_H) / (m * fin_H)
    
    # 5. 最終的な対外気熱抵抗 R_sa [K/W]
    R_sa = 1.0 / (h * A_total * fin_efficiency)
    
    return R_sa

# ==========================================
# 3. 初期化とシミュレーションループの実行
# ==========================================
t_arr = np.linspace(0, t_end, time_steps)

# 4つのノード温度 [Chip, VRAM, VRM, Sink] を格納する行列 (4 x time_steps)
T = np.zeros((4, time_steps))
T[:, 0] = Ta  # 初期温度はすべて外気温度と同じ

# 評価データ記録用配列
fan_pwm = np.zeros(time_steps)
P_history = np.zeros((3, time_steps)) # [P_chip, P_vram, P_vrm]
R_sa_history = np.zeros(time_steps)

integral_error = 0.0

print("シミュレーションを実行中...")

for i in range(0, time_steps - 1):
    t = t_arr[i]
    
    # 動的な発熱プロファイル（30秒後にGPUボード全体がフル稼働、180秒後に急にアイドルに戻る）
    if 30.0 < t < 180.0:
        p_chip = 200.0  # GPUコア 200W
        p_vram = 40.0   # ビデオメモリ 40W
        p_vrm  = 30.0   # 電源回路 30W
    else:
        p_chip, p_vram, p_vrm = 15.0, 5.0, 3.0 # アイドル負荷
        
    P_history[:, i] = [p_chip, p_vram, p_vrm]
    
    # 現在の各ノードの温度を取得
    t_chip, t_vram, t_vrm, t_sink = T[:, i]
    
    # --- ① 制御ロジック (GPU温度からファンのPWMを決定) ---
    error = t_chip - T_target
    integral_error += error * dt
    pwm = np.clip(Kp * error + Ki * integral_error, 0.0, 1.0)
    fan_pwm[i] = pwm
    
    # --- ② 流体力学フェーズ (PWM ➔ 風速 ➔ 熱伝達率 ➔ 熱抵抗を動的計算) ---
    R_sa = calculate_r_sa(pwm)
    R_sa_history[i] = R_sa

    # --- ③ 熱等価回路のマトリクス（行列）係数更新 ---
    # 4×4 の熱伝導状態マトリクス A
    A = np.array([
        [-1/(C_chip*R_chip_s),  0,                     0,                     1/(C_chip*R_chip_s)],
        [0,                     -1/(C_vram*R_vram_s),  0,                     1/(C_vram*R_vram_s)],
        [0,                     0,                     -1/(C_vrm*R_vrm_s),    1/(C_vrm*R_vrm_s)],
        [1/(C_sink*R_chip_s),   1/(C_sink*R_vram_s),   1/(C_sink*R_vrm_s),   -(1/R_chip_s + 1/R_vram_s + 1/R_vrm_s + 1/R_sa)/C_sink]
    ])
    
    # 入力電力マッピングマトリクス B
    B = np.array([
        [1/C_chip, 0,        0],
        [0,        1/C_vram, 0],
        [0,        0,        1/C_vrm],
        [0,        0,        0]
    ])
    
    # 外気温度ベクトル C
    C = np.array([0, 0, 0, (Ta / (C_sink * R_sa))])
    
    # --- ④ 行列演算による微分方程式の解法（オイラー法） ---
    T_vec = T[:, i]
    P_vec = np.array([p_chip, p_vram, p_vrm])
    
    dT_dt = (A @ T_vec) + (B @ P_vec) + C
    
    # 次のタイムステップの温度を決定
    T[:, i+1] = T[:, i] + dT_dt * dt

# 最終ステップのログの穴埋め
fan_pwm[-1] = fan_pwm[-2]
R_sa_history[-1] = R_sa_history[-2]
P_history[:, -1] = P_history[:, -2]

print("計算完了。グラフを描画します。")

# ==========================================
# 4. 可視化と評価（レポート画面）
# ==========================================
fig, (ax1, ax2, ax3) = plt.subplots(3, 1, figsize=(10, 11), sharex=True)

# グラフ1: 各電子部品とヒートシンクの温度
ax1.plot(t_arr, T[0, :], 'r-', linewidth=2, label='GPU Core (Chip)')
ax1.plot(t_arr, T[1, :], 'm-', linewidth=2, label='VRAM')
ax1.plot(t_arr, T[2, :], 'y-', linewidth=2, label='VRM')
ax1.plot(t_arr, T[3, :], 'b-', linestyle='--', label='Heatsink')
ax1.axhline(T_target, color='gray', linestyle=':', label='Target Temp (70°C)')
ax1.set_ylabel('Temperature [°C]')
ax1.grid(True, linestyle='--', alpha=0.5)
ax1.legend(loc='upper right')
ax1.set_title('Ultimate Multi-Physics Thermal Simulation', fontsize=14, fontweight='bold')

# グラフ2: ファンPWMと電力プロファイル
ax2.plot(t_arr, fan_pwm * 100, 'g-', linewidth=2, label='Fan PWM Duty (%)')
ax2.set_ylabel('Fan PWM [%]', color='green')
ax2.tick_params(axis='y', labelcolor='green')
ax2_twin = ax2.twinx()
ax2_twin.plot(t_arr, np.sum(P_history, axis=0), 'k:', alpha=0.5, label='Total Board Power (W)')
ax2_twin.set_ylabel('Total Power [W]')
ax2.grid(True, linestyle='--', alpha=0.5)
ax2.legend(loc='upper left')
ax2_twin.legend(loc='upper right')

# グラフ3: 流体力学計算によってリアルタイム変化したヒートシンクの熱抵抗 R_sa
ax3.plot(t_arr, R_sa_history, 'c-', linewidth=2, label='Calculated R_sa (Convection)')
ax3.set_xlabel('Time [seconds]')
ax3.set_ylabel('Thermal Resistance R_sa [K/W]')
ax3.grid(True, linestyle='--', alpha=0.5)
ax3.legend(loc='upper right')

plt.tight_layout()
plt.show()