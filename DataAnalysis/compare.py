import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.font_manager as fm

# =========================
# 中文字体：直接使用 Windows 字体文件
# =========================
font_path = "/mnt/c/Windows/Fonts/msyh.ttc"   # 微软雅黑
font_prop = fm.FontProperties(fname=font_path)

# 负号正常显示
plt.rcParams["axes.unicode_minus"] = False

# =========================
# 数据路径（这里改你的路径）
# =========================
DATA_DIR = "results/2/"   # ⭐ 数据文件夹路径

RL_FILE = DATA_DIR + "RL2_describe.csv"   # ⭐ RL数据
RR_FILE = DATA_DIR + "RR_describe.csv"    # ⭐ RR数据

# =========================
# 读取数据
# =========================
rl_df = pd.read_csv(RL_FILE, index_col=0)
rr_df = pd.read_csv(RR_FILE, index_col=0)

# 取均值和标准差
rl_mean = rl_df.loc["mean"]
rr_mean = rr_df.loc["mean"]

rl_std = rl_df.loc["std"]
rr_std = rr_df.loc["std"]

# =========================
# 只保留三个指标
# =========================
metrics_map = {
    "avg_wait": "平均等待时间（ticks）",
    "avg_turnaround": "平均周转时间（ticks）",
    "avg_response": "平均响应时间（ticks）"
}

metrics = list(metrics_map.keys())
labels = list(metrics_map.values())

# 构建数据
rl_mean_vals = [rl_mean[m] for m in metrics]
rr_mean_vals = [rr_mean[m] for m in metrics]

rl_std_vals = [rl_std[m] for m in metrics]
rr_std_vals = [rr_std[m] for m in metrics]

# =========================
# 误差棒修正：下界不低于 0
# =========================
rl_lower_err = [min(m, s) for m, s in zip(rl_mean_vals, rl_std_vals)]
rl_upper_err = rl_std_vals

rr_lower_err = [min(m, s) for m, s in zip(rr_mean_vals, rr_std_vals)]
rr_upper_err = rr_std_vals

# Matplotlib 非对称误差棒格式：[lower, upper]
rl_yerr = [rl_lower_err, rl_upper_err]
rr_yerr = [rr_lower_err, rr_upper_err]

# =========================
# 绘图
# =========================
x = list(range(len(metrics)))
width = 0.35

fig, ax = plt.subplots(figsize=(9, 5.5))

# RL
ax.bar(
    [i - width / 2 for i in x],
    rl_mean_vals,
    width=width,
    yerr=rl_yerr,
    capsize=5,
    label="QL-Scheduler"
)

# RR
ax.bar(
    [i + width / 2 for i in x],
    rr_mean_vals,
    width=width,
    yerr=rr_yerr,
    capsize=5,
    label="Round Robin"
)

# 坐标轴与标题
ax.set_xticks(x)
ax.set_xticklabels(labels, rotation=15, fontproperties=font_prop)
ax.set_ylabel("时间（ticks）", fontproperties=font_prop)
ax.set_title("模式2调度算法性能对比（均值 ± 标准差）", fontproperties=font_prop)

# 强制 y 轴从 0 开始
ax.set_ylim(bottom=0)

# 设置 y 轴刻度字体
for label in ax.get_yticklabels():
    label.set_fontproperties(font_prop)

# 图例字体
legend_prop = fm.FontProperties(fname=font_path, size=10)
ax.legend(prop=legend_prop)

plt.tight_layout()

# =========================
# 保存图像
# =========================
OUTPUT_PATH = DATA_DIR + "调度算法性能对比.png"   # ⭐ 输出路径
plt.savefig(OUTPUT_PATH, dpi=300, bbox_inches="tight")

plt.show()

print(f"\n图已保存到: {OUTPUT_PATH}")