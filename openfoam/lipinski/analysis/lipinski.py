# ケース名 -> [jL [cm/s], jG [cm/s], 実験 dP/dz [kPa/m], 実験 α]
CASES = {
    "case1": [10.23086773,  1.450982602,  52.51, 0.076271088],
    "case2": [9.90800949,  7.191437466,  72.32, 0.227619547],
    "case3": [10.27359597, 16.61999796,   97.79, 0.335288667],
    "case4": [10.05553458, 31.78596886,  111.62, 0.407000376],
    "case5": [9.939255262, 47.16244543, 120.91, 0.445500735],
    "case6": [10.11581402, 61.11335697,  135.88, 0.466643065],
    "case7": [9.952883534, 74.07770083, 140.69, 0.490540403],
    "case8": [10.29045802, 103.8733468,  167.84, 0.526286171],
}
EPS, DP, G = 0.38, 0.005, 9.81
RHO_L, MU_L = 998.2, 1.002e-3
RHO_G, MU_G = 1.205, 1.82e-5


def grad(alpha, j, rho, mu, phase):
    s = (1 - alpha) if phase == "L" else alpha
    visc = 150 * mu * j * (1 - EPS) ** 2 / (s ** 3 * EPS ** 3 * DP ** 2)
    iner = 1.75 * (1 - EPS) * rho * j ** 2 / (s ** 3 * EPS ** 3 * DP)
    return rho * G + visc + iner


def residual(alpha, jl, jg):
    return grad(alpha, jl, RHO_L, MU_L, "L") - grad(alpha, jg, RHO_G, MU_G, "G")


def solve(jl, jg, tol=1.0e-10):
    lo, hi = 1e-4, 1-1e-4
    for _ in range(200):
        mid = (lo + hi) / 2
        if residual(mid, jl, jg) > 0:
            hi = mid
        else:
            lo = mid
        if hi-lo < tol:
            break

    alpha = (lo + hi) / 2
    return alpha, grad(alpha, jl, RHO_L, MU_L, "L")


def run(name):
    jl_cm, jg_cm, exp_dp, exp_alpha = CASES[name]
    alpha, dpdz = solve(jl_cm / 100, jg_cm / 100)
    return alpha, dpdz/1000, exp_dp, exp_alpha


print("case    α(Lip)  α(実験)  α誤差   dP(Lip)  dP(実験)  dP誤差")
for name in CASES:
    a, lip, exp_dp, exp_a = run(name)
    print(f"{name:6s}  {a:6.4f}  {exp_a:6.4f}  {(a-exp_a)/exp_a*100:+6.1f}%  "
          f"{lip:7.2f}  {exp_dp:7.2f}  {(lip-exp_dp)/exp_dp*100:+6.1f}%")