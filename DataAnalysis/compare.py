import pandas as pd
import matplotlib.pyplot as plt

PREFIX = "results/"
RL_FILE = PREFIX + "RL_describe.csv"
RR_FILE = PREFIX + "RR_describe.csv"

rl_df = pd.read_csv(RL_FILE, index_col=0)
rr_df = pd.read_csv(RR_FILE, index_col=0)

rl_mean = rl_df.loc["mean"]
rr_mean = rr_df.loc["mean"]

avg_cols = [col for col in rl_df.columns if col.startswith("avg_")]

compare_df = pd.DataFrame({
    "metric": avg_cols,
    "RL": [rl_mean[col] for col in avg_cols],
    "RR": [rr_mean[col] for col in avg_cols],
})

compare_df["improvement_pct"] = (compare_df["RR"] - compare_df["RL"]) / compare_df["RR"] * 100

print(compare_df.to_string(index=False))

# 画图
x = range(len(avg_cols))
width = 0.35

plt.figure(figsize=(10, 6))
plt.bar([i - width/2 for i in x], compare_df["RL"], width=width, label="RL")
plt.bar([i + width/2 for i in x], compare_df["RR"], width=width, label="RR")

plt.xticks(list(x), compare_df["metric"], rotation=30)
plt.ylabel("Mean Value")
plt.title("Comparison of Avg Metrics: RL vs RR")
plt.legend()
plt.tight_layout()

plt.savefig("results/RL_vs_RR_avg_compare.png", dpi=300)
plt.show()

print("\n图已保存到: RL_vs_RR_avg_compare.png")