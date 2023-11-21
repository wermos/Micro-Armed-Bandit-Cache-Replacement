import argparse
import numpy as np
import matplotlib.pyplot as plt

from common import parse_ipc

lru_data, drrip_data, ship_data, hawkeye_data, rlr_data = parse_ipc()

# Extracting data for plotting
trace_names = list(lru_data.keys())

lru_values = list(lru_data.values())
drrip_values = list(drrip_data.values())
ship_values = list(ship_data.values())
hawkeye_values = list(hawkeye_data.values())
rlr_values = list(rlr_data.values())

# Calculate speedup values
for i in range(len(lru_values)):
    drrip_values[i] = drrip_values[i] / lru_values[i] - 1
    ship_values[i] = ship_values[i] / lru_values[i] - 1
    hawkeye_values[i] = hawkeye_values[i] / lru_values[i] - 1
    rlr_values[i] = rlr_values[i] / lru_values[i] - 1

# Sort data by increasing order of the maximum speedup value
sorted_indices = np.argsort(np.maximum.reduce([drrip_values, ship_values, hawkeye_values, rlr_values]))
trace_names = np.array(trace_names)[sorted_indices]
lru_values = np.array(lru_values)[sorted_indices]
drrip_values = np.array(drrip_values)[sorted_indices]
ship_values = np.array(ship_values)[sorted_indices]
hawkeye_values = np.array(hawkeye_values)[sorted_indices]
rlr_values = np.array(rlr_values)[sorted_indices]

fig, ax = plt.subplots(layout="constrained", figsize=(16, 9))

index = np.arange(len(trace_names))

# Plot lines for max values greater than 1 and min values less than 1
max_values = np.maximum.reduce([drrip_values, ship_values, hawkeye_values, rlr_values])
min_values = np.minimum.reduce([drrip_values, ship_values, hawkeye_values, rlr_values])

# Shade the area between the lines
plt.fill_between(index, max_values, min_values, color='gray', alpha=0.3)

plt.plot(index, max_values, marker='o', linestyle='--', color='black', label='Best-performing policy', linewidth=2)
plt.plot(index, min_values, marker='x', linestyle='--', color='red', label='Worst-performing policy', linewidth=2)

# Dotted horizontal line at 0
ax.axhline(0, color='gray', linestyle='dashed', linewidth=1)

# Annotate with names of data that gave highest and lowest values
for i, (max_val, min_val) in enumerate(zip(max_values, min_values)):
    if max_val == drrip_values[i]:
        max_name = "DRRIP"
    elif max_val == ship_values[i]:
        max_name = "SHiP"
    elif max_val == hawkeye_values[i]:
        max_name = "Hawkeye"
    elif max_val == rlr_values[i]:
        max_name = "RLR"

    ax.annotate(f'{max_name}', (index[i], max_val), textcoords="offset points", xytext=(0,15), ha='center', rotation=90)

# Add light gridlines
ax.grid(which='both', color='lightgray', linestyle='--', linewidth=0.5)
ax.yaxis.set_minor_locator(plt.MultipleLocator(0.25))

plt.xlabel('Trace File')
plt.ylabel('Speedup Over LRU')
plt.xticks(index, trace_names, fontsize=15, rotation=90)

# Set the lower bound for the y-axis
ax.set_ylim(bottom=-0.75)
ax.get_yaxis().set_major_formatter(plt.FuncFormatter(lambda x, loc: f"{x+1:.2f}"))

# Set x-axis limits to remove empty space on both sides
ax.set_xlim(left=-0.5, right=len(trace_names)-0.5)

plt.legend(loc="lower left")


parser = argparse.ArgumentParser()

parser.add_argument('-s', '--save', action='store_true', default=False)

args = parser.parse_args()

if args.save:
    plt.savefig("images/max-min.png")
    plt.savefig("images/max-min.pdf")
else:
    plt.show()
