import json,math,os
import numpy as np
import matplotlib.pyplot as plt

plt.rcParams['font.sans-serif'] = ['WenQuanYi Micro Hei Mono', 'WenQuanYi Zen Hei Mono,']
plt.rcParams['axes.unicode_minus'] = False

# 获取脚本所在目录
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))

# 核心参数顺序（必须与雷达数据中的键一致）
CORE_PARAMS = [
    "装填", "前进能力", "倒车速度", "双机响应速度",
    "防护面积", "弱点分布", "生存性", "小范围综合机动性",
    "穿深", "辅助设备"
]

# 玩家层次样式
LEVEL_STYLES = {
    "高手": {"color": "#E74C3C", "linestyle": "-", "marker": "o", "label": "高手"},
    "一般玩家": {"color": "#2ECC71", "linestyle": "--", "marker": "s", "label": "一般玩家"},
    "菜鸟": {"color": "#3498DB", "linestyle": "-.", "marker": "^", "label": "菜鸟"},
    "综合": {"color": "black", "linestyle": ":", "marker": "D", "label": "综合"},  # 新增
}

# 输出目录
OUTPUT_DIR = os.path.join(SCRIPT_DIR, "output")

# 加载雷达数据
def load_radar_data():
    file_path = os.path.join(SCRIPT_DIR, "radar_data.json")
    with open(file_path, "r", encoding="utf-8") as f:
        return json.load(f)

# 绘制单个车辆的雷达图
def plot_single_vehicle(vehicle_name, vehicle_data, params):
    num_params = len(params)
    angles = np.linspace(0, 2 * math.pi, num_params, endpoint=False).tolist()
    angles += angles[:1]  # 闭合

    fig, ax = plt.subplots(figsize=(10, 10), subplot_kw=dict(polar=True))
    fig.suptitle(f"{vehicle_name} 三层视角雷达图", fontsize=18, y=0.95)

    for level, style in LEVEL_STYLES.items():
        if level not in vehicle_data:
            continue
        values_dict = vehicle_data[level]
        values = [values_dict.get(p, 0) for p in params]
        values += values[:1]
        ax.plot(angles, values,
                color=style["color"],
                linestyle=style["linestyle"],
                marker=style["marker"],
                linewidth=2,
                markersize=6,
                label=style["label"])
        ax.fill(angles, values, color=style["color"], alpha=0.05)

    ax.set_xticks(angles[:-1])
    ax.set_xticklabels(params, fontsize=12)
    ax.set_rlabel_position(30)
    ax.set_yticks([2, 4, 6, 8, 10])
    ax.set_yticklabels(['2', '4', '6', '8', '10'], fontsize=8, color='gray')
    # 计算当前车辆所有层次、所有参数的最大值
    all_vals = []
    for level in ["高手", "一般玩家", "菜鸟", "综合"]:
        if level in vehicle_data:
            all_vals.extend(vehicle_data[level].values())
    local_max = max(all_vals) if all_vals else 10
    # 设置上限为局部最大值的 1.1 倍，并至少为 2 避免空图
    ax.set_ylim(0, max(local_max * 1.1, 2))

    # 加减分项
    bonuses = vehicle_data.get("加减分", {})
    if bonuses:
        bonus_text = " | ".join([f"{k}: {v:+.1f}" for k, v in bonuses.items()])
        ax.text(0.5, -0.15, f"加减分项: {bonus_text}",
                transform=ax.transAxes, ha='center', fontsize=10,
                bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.5))

    ax.legend(loc='upper right', bbox_to_anchor=(1.3, 1.1), fontsize=12)
    plt.tight_layout()

    os.makedirs(OUTPUT_DIR, exist_ok=True)
    filename = os.path.join(OUTPUT_DIR, f"{vehicle_name}_雷达图.png")
    plt.savefig(filename, dpi=150, bbox_inches='tight')
    print(f"已生成: {filename}")
    plt.close()

# 绘制多车辆对比雷达图
def plot_comparison(radar_data, params):
    # 1. 先提取所有载具名称
    vehicle_names = list(radar_data.keys())
    if len(vehicle_names) < 2:
        return

    # 2. 计算全局最大值（为所有载具所有层次所有参数中的最大值）
    global_max = 0
    for vname in vehicle_names:
        for level in ["高手", "一般玩家", "菜鸟"]:
            if level in radar_data[vname]:
                vals = radar_data[vname][level].values()
                if vals:
                    global_max = max(global_max, max(vals))
    global_max = max(global_max * 1.1, 2)

    # 3. 准备画图
    num_params = len(params)
    angles = np.linspace(0, 2 * math.pi, num_params, endpoint=False).tolist()
    angles += angles[:1]

    fig, axes = plt.subplots(1, 3, figsize=(24, 8), subplot_kw=dict(polar=True))
    fig.suptitle("多车载具三层视角对比", fontsize=20, y=1.02)

    for idx, level in enumerate(["高手", "一般玩家", "菜鸟"]):
        ax = axes[idx]
        ax.set_title(level, fontsize=16, pad=20)
        for v_name in vehicle_names:
            if level not in radar_data[v_name]:
                continue
            values_dict = radar_data[v_name][level]
            values = [values_dict.get(p, 0) for p in params]
            values += values[:1]
            ax.plot(angles, values, linewidth=2, label=v_name)
            ax.fill(angles, values, alpha=0.05)
        ax.set_xticks(angles[:-1])
        ax.set_xticklabels(params, fontsize=10)
        ax.set_ylim(0, global_max)
        ax.legend(loc='upper right', bbox_to_anchor=(1.4, 1.1), fontsize=9)

    plt.tight_layout()
    filename = os.path.join(OUTPUT_DIR, "多车对比_雷达图.png")
    plt.savefig(filename, dpi=150, bbox_inches='tight')
    print(f"已生成对比图: {filename}")
    plt.close()

if __name__ == "__main__":
    radar_data = load_radar_data()

    for vehicle_name, vdata in radar_data.items():
        plot_single_vehicle(vehicle_name, vdata, CORE_PARAMS)

    if len(radar_data) >= 2:
        plot_comparison(radar_data, CORE_PARAMS)

    print("所有预览图生成完毕！")
