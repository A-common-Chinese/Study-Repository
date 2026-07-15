```markdown
# War Thunder Top-Tier MBT Evaluation Radar Charts

A quantitative, multi-perspective evaluation system for comparing top-tier main battle tanks in *War Thunder*.  
It takes raw in‑game measurements, converts them into 0–10 scores, applies a **dual‑coefficient player skill model**, and generates spider charts showing how well a vehicle **fits** three different player types:

- **Elite (高手)** – high mechanical skill, values firepower and mobility
- **Average (一般玩家)** – balanced needs
- **Beginner (菜鸟)** – relies heavily on armor and survivability

## Table of Contents
- [Features](#features)
- [Project Structure](#project-structure)
- [Requirements](#requirements)
- [Quick Start](#quick-start)
- [Usage](#usage)
- [Configuration](#configuration)
- [How It Works](#how-it-works)
- [Packaging for Distribution](#packaging-for-distribution)
- [License](#license)

## Features
- **Raw → Score automation** – drop your in‑game measurements into `vehicles_raw.json`, run one script, and get 0–10 scores.
- **Dual‑coefficient model** – *utilization coefficient* (how well a player can exploit a parameter) and *dependency coefficient* (how much a player relies on it) are combined to compute **player‑specific fit scores**.
- **12 evaluation dimensions** – reload, forward mobility, reverse speed, gun handling, armor coverage, weakspot layout, survivability, agility, penetration, equipment, gun depression, and defensive electronics (APS/LWS).
- **Four radar chart layers** – Elite, Average, Beginner, and Overall (average of all three).
- **Multi‑vehicle comparison** – side‑by‑side radar grids for easy comparison.
- **JSON‑based workflow** – all parameters and weights are editable without touching code.
- **One‑click pipeline** – from raw data to charts with a single shell command.

## Project Structure
```
权重/
├── vehicles_raw.json          # Raw in‑game measurements (YOU fill this)
├── convert_rules.json         # Mapping rules: raw values → 0–10 scores
├── convert_raw.py             # Conversion script
├── weights.json               # Player skill coefficients (utilization & dependency)
├── weight_calculator.py       # Computes “fit scores” per player level
├── vehicles_quantified.json   # Auto‑generated 0–10 scores
├── radar_data.json            # Auto‑generated radar chart coordinates
├── preview_radar.py           # Generates PNG radar charts
├── run_all.sh                 # One‑click pipeline (Linux/macOS)
├── output/                    # Generated radar charts
│   ├── T-90M_radar.png
│   ├── ZTZ99A_radar.png
│   ├── M1A2SEP_radar.png
│   └── 多车对比_radar.png
└── venv/                      # Python virtual environment (optional)
```

## Requirements
- Python 3.8+
- Required packages: `numpy`, `matplotlib`
- A virtual environment is recommended (see Quick Start)

## Quick Start

### 1. Clone or download the repository
```bash
git clone <your-repo-url>  # or download the ZIP
cd 权重
```

### 2. Set up a virtual environment (recommended)
```bash
python3 -m venv venv
source venv/bin/activate      # Linux/macOS
# On Windows: venv\Scripts\activate
pip install numpy matplotlib
```

### 3. Prepare raw data
Edit `vehicles_raw.json` with your in‑game measurements for each tank.  
Example fields:
```json
"T-90M": {
  "装填时间_s": 7.0,
  "前进赛道时间_s": 101.0,
  "倒车极速_kmh": 4,
  "方向机转速": 34,
  "高低机转速": 32,
  "弱区占比": 0.30,
  "车体投影面积": 0.5,
  "弱点聚落数": 2,
  "离散度修正": 0,
  "形状加分": 1.0,
  "生存性_被穿后存活率": 0.3,
  "绕桩时间_s": 18.2,
  "原地转向时间_s": 9.5,
  "急弯加减速时间_s": 15.1,
  "穿深": 580,
  "辅助设备档次": "最好",
  "俯角_度": -6,
  "APS": false,
  "LWS": false
}
```
*(See `vehicles_raw.json` example in the repository.)*

### 4. Run the full pipeline
```bash
chmod +x run_all.sh
./run_all.sh
```
This will:
1. Convert raw measurements to 0–10 scores (`convert_raw.py`)
2. Compute player‑specific fit scores (`weight_calculator.py`)
3. Generate all radar charts (`preview_radar.py`)

Output images are saved in the `output/` folder.

### 5. (Optional) Run individual steps
```bash
python3 convert_raw.py
python3 weight_calculator.py
python3 preview_radar.py
```

## Usage
- **Add a new tank**: append its data to `vehicles_raw.json` and re‑run `./run_all.sh`.
- **Tweak conversion rules**: edit `convert_rules.json` (thresholds, mapping types). No code changes needed.
- **Adjust player weights**: edit `weights.json` (utilization & dependency coefficients). Then re‑run `weight_calculator.py` and `preview_radar.py`.
- **Manually override a score**: edit `vehicles_quantified.json` directly (but it will be overwritten the next time you run `convert_raw.py`).

## Configuration

### `convert_rules.json`
Defines how each raw parameter is turned into a 0–10 score. Supported types:
- `higher_better_linear` / `lower_better_linear` (with `best` / `worst` boundaries)
- `higher_better_sigmoid` / `lower_better_sigmoid` (with `center` / `scale`)
- `categorical` (with a `mapping` dictionary)
- `raw` – passed through as‑is
- `custom` – handled by special logic in `convert_raw.py` (e.g., weakspot topology)

### `weights.json`
Contains **utilization coefficients** and **dependency coefficients** for each player level across 10 core parameters.  
The coefficients are used directly in the “fit score” formula (no extra normalization). See [How It Works](#how-it-works) for details.

## How It Works

### 1. Raw → Score conversion
Each raw measurement (e.g., reload time, track traverse) is mapped to a 0–10 score using the rules defined in `convert_rules.json`. Some scores are combined (e.g., turret rotation + elevation speed → “gun handling”).

### 2. Player‑specific fit score
For each tank, parameter, and player level (`高手`, `一般玩家`, `菜鸟`):
```
fit_score = objective_score × (1 + k × speciality)
```
where:
- `objective_score` is from `vehicles_quantified.json`
- `speciality = |utilization - 1| + |dependency - 1|`
- `k` is a scaling factor (default 1.0) that controls how much “special” parameters are amplified.

**Interpretation**:  
If a player is extremely good at using a parameter (utilization far from 1) or extremely dependent on it (dependency far from 1), the car’s score in that area gets **amplified** – because a strong car in a “special” area is a bigger advantage (or a pleasant surprise).  
The result is an intuitively “bigger is better” radar chart: a large branch means the car is excellent for that player in that aspect.

### 3. Overall score
The “Overall” layer is the arithmetic mean of the three player levels.

### 4. Radar chart generation
`preview_radar.py` reads `radar_data.json` and draws:
- A single‑vehicle spider chart with four layers (Elite, Average, Beginner, Overall)
- A 4‑panel comparison chart showing all vehicles side‑by‑side for each player level.

## Packaging for Distribution
If you want to share the tool with users who don’t have Python, you can bundle it into a standalone executable using **PyInstaller**.

1. Merge all scripts into a single `main.py` (or keep the pipeline architecture).
2. Install PyInstaller: `pip install pyinstaller`
3. Run:
   ```bash
   pyinstaller --onefile --add-data "weights.json:." --add-data "convert_rules.json:." --add-data "vehicles_raw.json:." --hidden-import matplotlib --hidden-import numpy main.py
   ```
The resulting executable in `dist/` can run on a machine without Python.

> **Note**: The output folder will be created relative to the executable’s location. If you want external configuration files, adjust the code to look for them next to the executable first.

## License
This project is provided for educational and community purposes. Feel free to use, modify, and share with attribution.
```
