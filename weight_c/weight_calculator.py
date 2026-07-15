import json,os,math

# 获取脚本所在的目录
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))

def create_dict(file_name="weights.json"):
    # 拼接出与脚本同目录的文件完整路径
    file_path = os.path.join(SCRIPT_DIR, file_name)
    with open(file_path, 'r', encoding='utf-8') as file:
        data = json.load(file)
    return data

# 如果 weights.json 不存在，自动创建一个示例文件
def ensure_weights_exist(file_name="weights.json"):
    file_path = os.path.join(SCRIPT_DIR, file_name)
    if not os.path.exists(file_path):
        sample_data = {
            "发挥系数": {
                "高手": {"穿深": 1, "装填": 2, "辅助设备": 1, "双机响应速度": 2,
                        "弱点分布": 1, "防护面积": 1, "生存性": 2, "倒车速度": 2,
                        "前进能力": 2, "小范围综合机动性": 2},
                "一般玩家": {"穿深": 1, "装填": 1, "辅助设备": 1, "双机响应速度": 1,
                          "弱点分布": 1, "防护面积": 1, "生存性": 1, "倒车速度": 1,
                          "前进能力": 1, "小范围综合机动性": 1},
                "菜鸟": {"穿深": 1, "装填": 0.5, "辅助设备": 0.5, "双机响应速度": 0.5,
                        "弱点分布": 0.5, "防护面积": 1, "生存性": 0.5, "倒车速度": 0.5,
                        "前进能力": 0.5, "小范围综合机动性": 0.5}
            },
            "依赖系数": {
                "高手": {"穿深": 0.5, "装填": 0.5, "辅助设备": 0.5, "双机响应速度": 0.5,
                        "弱点分布": 0.5, "防护面积": 0.5, "生存性": 1, "倒车速度": 1,
                        "前进能力": 2, "小范围综合机动性": 2},
                "一般玩家": {"穿深": 1, "装填": 1, "辅助设备": 1, "双机响应速度": 1,
                          "弱点分布": 1, "防护面积": 1, "生存性": 1, "倒车速度": 1,
                          "前进能力": 1, "小范围综合机动性": 2},
                "菜鸟": {"穿深": 2, "装填": 2, "辅助设备": 2, "双机响应速度": 2,
                        "弱点分布": 2, "防护面积": 2, "生存性": 2, "倒车速度": 2,
                        "前进能力": 0.5, "小范围综合机动性": 0.5}
            }
        }
        with open(file_path, 'w', encoding='utf-8') as f:
            json.dump(sample_data, f, ensure_ascii=False, indent=2)
        print(f"已自动创建示例文件：{file_path}")

# 如果 vehicles_quantified.json 不存在，自动创建一个示例文件
def ensure_vehicles_exist(file_name="vehicles_quantified.json"):
    file_path = os.path.join(SCRIPT_DIR, file_name)
    if not os.path.exists(file_path):
        sample_data = {
            "T-90M": {
                "装填": 3.8,
                "前进能力": 6.2,
                "倒车速度": 2.0,
                "双机响应速度": 6.5,
                "防护面积": 8.0,
                "弱点分布": 7.5,
                "生存性": 6.0,
                "小范围综合机动性": 5.5,
                "穿深": 6.0,
                "辅助设备": 10,
                "俯角": 3.0,
                "APS": 0.0,
                "LWS": 0.0
            },
            "ZTZ99A": {
                "装填": 3.8,
                "前进能力": 9.5,
                "倒车速度": 9.5,
                "双机响应速度": 7.5,
                "防护面积": 5.0,
                "弱点分布": 7.5,
                "生存性": 4.0,
                "小范围综合机动性": 7.5,
                "穿深": 5.9,
                "辅助设备": 10,
                "俯角": 3.2,
                "APS": 0.0,
                "LWS": 0.0
            },
            "M1A2SEP": {
                "装填": 8.0,
                "前进能力": 9.5,
                "倒车速度": 8.0,
                "双机响应速度": 8.5,
                "防护面积": 5.0,
                "弱点分布": 7.5,
                "生存性": 7.0,
                "小范围综合机动性": 9.0,
                "穿深": 9.0,
                "辅助设备": 10,
                "俯角": 8.0,
                "APS": 0.0,
                "LWS": 0.0
            }
        }

def print_weight_data(data, max_depth=None, indent=0):
    #max_depth 控制最大深度，indent 控制缩进
    if max_depth is not None and max_depth <= 0:
        return
    if isinstance(data, dict):
        for key, value in data.items():
            print("  " * indent + str(key), end="")
            if isinstance(value, dict):
                print()                     # 换行，展开下一层
                print_weight_data(value, 
                                  max_depth - 1 if max_depth else None, 
                                  indent + 1)
            else:
                print(f" : {value}")        # 遍历到最底层,打印值
    else:
        print("  " * indent + str(data))

# 主程序
if __name__ == "__main__":
    ensure_weights_exist()  # 如果没有就创建
    weight_data = create_dict()

    # 将权重数据按玩家水平分类存储
    high_level_weight = {}
    for param in weight_data["发挥系数"]["高手"]:
        high_level_weight[param] = {
            "发挥系数": weight_data["发挥系数"]["高手"][param],
            "依赖系数": weight_data["依赖系数"]["高手"][param]
        }
    middle_level_weight = {}
    for param in weight_data["发挥系数"]["一般玩家"]:
        middle_level_weight[param] = {
            "发挥系数": weight_data["发挥系数"]["一般玩家"][param],
            "依赖系数": weight_data["依赖系数"]["一般玩家"][param]
        }
    low_level_weight = {}
    for param in weight_data["发挥系数"]["菜鸟"]:
        low_level_weight[param] = {
            "发挥系数": weight_data["发挥系数"]["菜鸟"][param],
            "依赖系数": weight_data["依赖系数"]["菜鸟"][param]
        }

    print_weight_data(weight_data,3)
    print("\n")

    # 计算各层次每个参数的总权重（未归一化）
    total_weights = {
        "高手": {},
        "一般玩家": {},
        "菜鸟": {}
    }
    for param in high_level_weight:
        f = high_level_weight[param]["发挥系数"]
        y = high_level_weight[param]["依赖系数"]
        total_weights["高手"][param] = f * math.sqrt(y)
    for param in middle_level_weight:
        f = middle_level_weight[param]["发挥系数"]
        y = middle_level_weight[param]["依赖系数"]
        total_weights["一般玩家"][param] = math.sqrt(f * y)
    for param in low_level_weight:
        f = low_level_weight[param]["发挥系数"]
        y = low_level_weight[param]["依赖系数"]
        total_weights["菜鸟"][param] = y * math.sqrt(f)

    # 归一化
    normalized_weight = {"高手": {}, "一般玩家": {}, "菜鸟": {}}
    for level in total_weights:
        s = sum(total_weights[level].values())
        for param in total_weights[level]:
            normalized_weight[level][param] = total_weights[level][param] / s


    print("\n总权重：")
    print_weight_data(normalized_weight, 2)

    ensure_vehicles_exist()
    vehicles_path = os.path.join(SCRIPT_DIR, 'vehicles_quantified.json')
    with open(vehicles_path, 'r', encoding='utf-8') as f:
        vehicle_scores = json.load(f)

    # 计算每辆车的雷达图数据
    radar_data = {}
    for tank, scores in vehicle_scores.items():
        radar_data[tank] = {"高手": {}, "一般玩家": {}, "菜鸟": {}, "加减分": {}}
        
        for level in ["高手", "一般玩家", "菜鸟"]:
            for param in normalized_weight[level]:
                # 核心参数加权
                if param in scores:
                    radar_data[tank][level][param] = round(
                        scores[param] * normalized_weight[level][param], 2
                    )
        
        # 加减分项单独存储
        for bonus in ["俯角", "APS", "LWS"]:
            if bonus in scores:
                radar_data[tank]["加减分"][bonus] = scores[bonus]

    # 保存雷达图数据
    radar_path = os.path.join(SCRIPT_DIR, 'radar_data.json')
    with open(radar_path, 'w', encoding='utf-8') as f:
        json.dump(radar_data, f, ensure_ascii=False, indent=2)
    print(f"雷达图数据已生成：{radar_path}")
