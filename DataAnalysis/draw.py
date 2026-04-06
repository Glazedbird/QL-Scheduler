import os
import pandas as pd
import matplotlib.pyplot as plt
import yaml
import argparse

## 常量加载
with open("config.yaml") as f:
    config = yaml.safe_load(f)

parser = argparse.ArgumentParser()
mode_path = str(config["mode"]) + "/"
mode = config["mode"]
## 本文件中常量
output_path = "./results/" + mode_path + config["model"] + "_describe.csv"
input_path = "./data/data" + config["model"] + ".csv"
fig_output_path_prefix = "./figures/" + mode_path + config["model"]

os.makedirs("./results/" + mode_path , exist_ok=True)
os.makedirs("./figures/" + mode_path, exist_ok=True)

df = pd.read_csv(input_path)
df1 = df[df["mode"] == mode].copy()

print("基本统计：")
stats = df1.describe()
print(stats)
# 保存统计结果
stats.to_csv(output_path, encoding="utf-8-sig")

x = range(len(df1))

avg_cols = ["avg_turnaround", "avg_response", "avg_wait", "avg_run", "avg_sched"]
total_cols = ["total_turnaround", "total_response", "total_wait", "total_run", "total_sched"]

# 平均指标折线图
for col in avg_cols:
    plt.figure(figsize=(8, 4))
    plt.plot(x, df1[col], marker="o")
    plt.xlabel("run index")
    plt.ylabel(col)
    plt.title(f"{col} over repeated runs (mode={mode})")
    plt.grid(True)
    plt.tight_layout()
    plt.savefig(f"{fig_output_path_prefix}/{col}_line.png", dpi=300, bbox_inches="tight")
    plt.close()

# 平均指标箱线图
for col in avg_cols:
    plt.figure(figsize=(5, 4))
    plt.boxplot(df1[col])
    plt.ylabel(col)
    plt.title(f"Boxplot of {col} (mode={mode})")
    plt.tight_layout()
    plt.savefig(f"{fig_output_path_prefix}/{col}_box.png", dpi=300, bbox_inches="tight")
    plt.close()

# 总量指标折线图
for col in total_cols:
    plt.figure(figsize=(8, 4))
    plt.plot(x, df1[col], marker="o")
    plt.xlabel("run index")
    plt.ylabel(col)
    plt.title(f"{col} over repeated runs (mode={mode})")
    plt.grid(True)
    plt.tight_layout()
    plt.savefig(f"{fig_output_path_prefix}/{col}_line.png", dpi=300, bbox_inches="tight")
    plt.close()

# 散点图
plt.figure(figsize=(6, 5))
plt.scatter(df1["total_sched"], df1["total_wait"])
plt.xlabel("total_sched")
plt.ylabel("total_wait")
plt.title("total_wait vs total_sched (mode={mode})")
plt.grid(True)
plt.tight_layout()
plt.savefig(f"{fig_output_path_prefix}/total_wait_vs_total_sched.png", dpi=300, bbox_inches="tight")
plt.close()

print("\n统计结果已保存到" + output_path + ".csv")
print("图像已保存到 figures/ 目录下。")