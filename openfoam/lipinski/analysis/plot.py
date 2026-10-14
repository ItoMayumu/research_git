import matplotlib.pyplot as plt
import pandas as pd
import matplotlib
from lipinski import CASES, run


df = pd.read_csv("./openfoam/lipinski/analysis/comparison.csv")
df["jG"] = df["case"].map(lambda c: CASES[c][1])
