import numpy as np

def read_scalar(path):
    with open(path) as f:
        lines = f.readlines()

    for i, line in enumerate(lines):
        if "internalField" in line:
            break

    n = int(lines[i+1])

    data = lines[i+3:i+3+n]
    return np.array(data, dtype=float)


def read_vector(path):
    with open(path) as f:
        lines = f.readlines()

    for i, line in enumerate(lines):
        if "internalField" in line:
            break

    n = int(lines[i+1])
    data = lines[i+3:i+3+n]

    # "(ux uy uz)" -> かっこを外して3つに分ける -> (n, 3) の配列
    rows = []
    for line in data:
        line = line.replace("(", "").replace(")", "")
        rows.append(line.split())
    return np.array(rows, dtype=float)


case = "/home/mayumu/research/numerical_simulation/openfoam/lipinski/cases/packedBed/paramStudy/case4_jL10p05553458_jG31p78596886/10.499113933828617"

# --- 読み込み ---
z = read_scalar(case + "/Cz")
V = read_scalar(case + "/V")
p = read_scalar(case + "/p")
alpha = read_scalar(case + "/alpha.air")
U = read_vector(case + "/U.air")

# --- セルごとの計算 ---
R = 8314.47 / 28.9      # 空気の気体定数 [J/(kg K)]
T = 298.15              # T.air は 298.143〜298.176 K なので固定
rhoG = p / (R * T)      # 気体密度 [kg/m3]
jG = alpha * U[:, 2]    # 見かけ流速 [m/s] (Uz を使う)
G = rhoG * jG           # 質量流束 [kg/(m2 s)]

# --- z ごとの断面平均 (セル体積で重み付け) ---
# 同じ高さのセルは Cz が同じ値になるので、丸めてから重複を除く
zr = np.round(z, 6)
zlist = np.unique(zr)

print(f"{'z[m]':>7} {'p[bar]':>7} {'rhoG':>7} {'jG[m/s]':>8} {'G[kg/m2s]':>10}")
for zz in zlist[::5]:
    m = (zr == zz)          # この高さのセルだけ True になるマスク
    w = V[m] / V[m].sum()   # 体積の重み (合計 1)
    print(f"{zz:7.3f} {(w*p[m]).sum()/1e5:7.3f} {(w*rhoG[m]).sum():7.3f}"
          f" {(w*jG[m]).sum():8.4f} {(w*G[m]).sum():10.4f}")
