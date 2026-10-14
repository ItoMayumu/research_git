import pandas as pd
from lipinski import CASES, run

rows = []
for name in CASES:
    a, dp, exp_dp, exp_alpha = run(name)
    rows.append({
        "case": name,
        "α(Lip)": a,
        "α(実験)": exp_alpha,
        "α誤差": (a-exp_alpha)/exp_alpha*100,
        "dP(Lip)": dp,
        "dP(実験)": exp_dp,
        "dP誤差": (dp-exp_dp)/exp_dp*100
    })
    
df = pd.DataFrame(rows)
print(df.round(4).to_string(index=False))
df.round(4).to_csv("./openfoam/lipinski/analysis/comparison.csv", index=False)
print("\nsaved: Comparison.csv")
