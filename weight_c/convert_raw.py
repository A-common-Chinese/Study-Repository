import json, math, os

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))

def load_json(filename):
    with open(os.path.join(SCRIPT_DIR, filename), 'r', encoding='utf-8') as f:
        return json.load(f)

def higher_better_linear(value, best, worst):
    return max(0, min(10, 10 * (value - worst) / (best - worst)))

def lower_better_linear(value, best, worst):
    return max(0, min(10, 10 * (worst - value) / (worst - best)))

def sigmoid(value, center, scale):
    return 10 / (1 + math.exp((value - center) / scale))

def higher_better_sigmoid(value, center, scale):
    return 10 - sigmoid(value, center, scale)

def lower_better_sigmoid(value, center, scale):
    return sigmoid(value, center, scale)

def categorical(value, mapping):
    return mapping.get(value, 0)

def main():
    raw = load_json('vehicles_raw.json')
    rules = load_json('convert_rules.json')

    quantified = {}

    for tank, props in raw.items():
        q = {}

        # 直接映射的参数
        q['装填'] = apply_rule(props, rules, '装填时间_s')
        q['前进能力'] = apply_rule(props, rules, '前进赛道时间_s')
        q['倒车速度'] = apply_rule(props, rules, '倒车极速_kmh')

        # 双机响应速度：取方向机和高低机的加权平均得分
        turret_score = apply_rule(props, rules, '方向机转速')
        elevation_score = apply_rule(props, rules, '高低机转速')
        q['双机响应速度'] = math.sqrt(1.5 * turret_score * elevation_score * 0.8)

        # 防护面积：结合弱区占比和车体投影面积
        r = props['弱区占比']
        a = props['车体投影面积']
        # 得分 = 20 * (1 - r*a)^2，拥抱极端
        q['防护面积'] = round(15 * (1 - r * a) ** 2, 2)

        # 弱点分布：聚落数+离散度修正+形状加分，然后映射到0-10
        clusters = props['弱点聚落数']
        dispersion = props['离散度修正']
        shape = props['形状加分']
        raw_weakspot = min(10, max(0, clusters * 2 + dispersion + shape))
        q['弱点分布'] = round(raw_weakspot, 2)

        # 生存性
        q['生存性'] = apply_rule(props, rules, '生存性_被穿后存活率')

        # 小范围综合机动性：绕桩、原地转向、急弯加减速加权
        slalom = apply_rule(props, rules, '绕桩时间_s')
        turn = apply_rule(props, rules, '原地转向时间_s')
        acc_brake = apply_rule(props, rules, '急弯加减速时间_s')
        q['小范围综合机动性'] = round(0.2*slalom + 0.3*turn + 0.5*acc_brake, 2)

        # 穿深
        q['穿深'] = apply_rule(props, rules, '穿深')

        # 辅助设备
        q['辅助设备'] = apply_rule(props, rules, '辅助设备档次')

        # 加减分项
        q['俯角'] = abs(props['俯角_度']) * 0.3  # 每度俯角加/减 0.3 分
        q['APS'] = 2.0 if props['APS'] else 0.0
        q['LWS'] = 1.0 if props['LWS'] else 0.0

        quantified[tank] = q

    # 保存
    out_path = os.path.join(SCRIPT_DIR, 'vehicles_quantified.json')
    with open(out_path, 'w', encoding='utf-8') as f:
        json.dump(quantified, f, ensure_ascii=False, indent=2)
    print(f'量化数据已生成：{out_path}')

def apply_rule(props, rules, key):
    if key not in props or key not in rules:
        return 0
    value = props[key]
    rule = rules[key]
    t = rule['type']
    if t == 'higher_better_linear':
        return round(higher_better_linear(value, rule['best'], rule['worst']), 2)
    elif t == 'lower_better_linear':
        return round(lower_better_linear(value, rule['best'], rule['worst']), 2)
    elif t == 'higher_better_sigmoid':
        return round(higher_better_sigmoid(value, rule['center'], rule['scale']), 2)
    elif t == 'lower_better_sigmoid':
        return round(lower_better_sigmoid(value, rule['center'], rule['scale']), 2)
    elif t == 'categorical':
        return round(categorical(value, rule['mapping']), 2)
    elif t == 'raw':
        return value  # 原样返回，用于后续自定义计算
    elif t == 'custom':
        return 0  # 这里不会进入，custom项在上层处理
    else:
        return 0

if __name__ == '__main__':
    main()
