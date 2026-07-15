Below is a complete `README.md` in English for your project. You can place it in the project folder alongside the scripts.

```markdown
# War Thunder Top-Tier MBT Evaluation Radar Chart

A quantitative, multi-perspective evaluation system for comparing top-tier main battle tanks in *War Thunder*.  
It generates spider (radar) charts showing how a vehicle performs for three different player skill levels:  
**Elite (高⼿)**, **Average (⼀般玩家)**, and **Beginner (菜鸟)**.

The model was developed from a detailed community debate analysis and separates a vehicle's **objective parameters** from how important those parameters are to different players. This makes the often-subjective "this tank is OP / garbage" arguments visible and comparable through data.

---

## Features

- **Three player perspectives** – Elite, Average, Beginner – each with different weighting schemes.
- **Dual coefficient system** – *Utilization coefficient* (how well a player can exploit a parameter) and *Dependency coefficient* (how much a player relies on it).
- **12 evaluation dimensions** – Reload, forward mobility, reverse speed, gun handling, armor coverage, weakspot layout, survivability, agility, penetration, equipment, gun depression, and defensive electronics (APS/LWS).
- **Separation of raw data and quantified scores** – Easily adjust scores without touching code.
- **Automatic radar chart generation** – Creates single-vehicle charts (three overlaid layers) and multi-vehicle comparison charts.
- **JSON-based workflow** – All data inputs and outputs are human-readable and editable.

---

## File Structure

```
├── weight_calculator.py        # Reads weights and vehicle scores, computes weighted radar coordinates
├── preview_radar.py            # Reads radar data and generates spider charts
├── weights.json                # Utilization & dependency coefficients for each player level
├── vehicles_quantified.json    # Objective 0–10 scores for each vehicle's parameters
├── vehicles_raw.json           # (Optional) raw measured data, for future automated scoring
├── radar_data.json             # Intermediate file: weighted coordinates, auto-generated
└── output/                     # Generated radar chart images
    ├── T-90M_雷达图.png
    ├── ZTZ99A_雷达图.png
    └── ...
```

---

## Requirements

- Python 3.8+
- [matplotlib](https://matplotlib.org/) (`pip install matplotlib`)
- Standard libraries: `json`, `os`, `math`

---

## Quick Start

### 1. Clone or download the repository
Place all files in the same directory.

### 2. Prepare input data
Two JSON files are required (they will be auto-created with example data if missing):
- **`weights.json`** – The player skill weight coefficients (utilization & dependency).
- **`vehicles_quantified.json`** – Your 0–10 scores for each tank on each parameter.

Example content of `vehicles_quantified.json`:
```json
{
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
  }
}
```
The keys must match the parameter names exactly:  
`装填, 前进能力, 倒车速度, 双机响应速度, 防护面积, 弱点分布, 生存性, 小范围综合机动性, 穿深, 辅助设备, 俯角, APS, LWS`.

### 3. Run the weight calculator
```bash
python3 "Weight Calculator.py"
```
This produces `radar_data.json` containing the final weighted values for all three player levels.

### 4. Generate radar charts
```bash
python3 "preview_radar.py"
```
All images will be saved in the `output/` folder.  

---

## Customisation

- **Adjusting weights** – Edit `weights.json` directly. The file is read by `Weight Calculator.py` every run.
- **Adding a new vehicle** – Append a new entry to `vehicles_quantified.json` with its parameter scores, then re-run the calculator and previewer.
- **Tweaking score scaling** – You can change the raw→score mapping (currently manual). A future script will convert raw measurements (from `vehicles_raw.json`) into quantified scores.
- **Changing radar chart colors / styles** – Modify the `LEVEL_STYLES` dictionary inside `preview_radar.py`.

---

## How the Weighting Works

For each parameter and each player level, a **total weight** is computed from two coefficients:

| Player level | Formula |
|--------------|---------|
| Elite        | `utilization × sqrt(dependency)` |
| Average      | `sqrt(utilization × dependency)` |
| Beginner     | `dependency × sqrt(utilization)` |

The weights are then **normalised** so they sum to 1 within each level.  
A vehicle’s spider-chart branch value = `objective_score × normalised_weight`.

Additionally, **gun depression**, **APS**, and **LWS** are treated as bonus/penalty points and shown on the chart outside the main axes.

---

## Background

This system originated from a lengthy debate on the Chinese *War Thunder* community about whether the T-90M or ZTZ-99A is better. The argument revealed that most disagreements came from different players implicitly weighting parameters differently.  
By making those weights explicit and separating them from objective tank stats, the model shows **why** the same vehicle can be seen as “godlike” by beginners and “trash” by experts – and quantifies the difference.

---

## Roadmap

- [ ] Script to auto-convert raw measurements (e.g. reload time, track traverse) into 0–10 scores
- [ ] Interactive web-based radar viewer
- [ ] Community-contributed vehicle database

---

## License

This project is provided for educational and community purposes. Feel free to use, modify, and share with attribution.
