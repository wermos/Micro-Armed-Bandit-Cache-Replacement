import os
import numpy as np
import matplotlib
import matplotlib.pyplot as plt

from common import parse_mpki

font = {"family" : "Ubuntu",
        "size"   : 22}
matplotlib.rc("font", **font)


lru_data, drrip_data, ship_data, hawkeye_data, rlr_data = parse_mpki()

# Extracting data for plotting
trace_names = list(lru_data.keys())

lru_values = list(lru_data.values())
drrip_values = list(drrip_data.values())
ship_values = list(ship_data.values())
hawkeye_values = list(hawkeye_data.values())
rlr_values = list(rlr_data.values())

fig, ax = plt.subplots(layout="constrained", figsize=(16, 9))

# Create a bar plot
bar_width = 0.2
index = np.arange(len(trace_names))

plt.bar(index, lru_values, bar_width, label='LRU')
plt.bar(index + bar_width, drrip_values, bar_width, label='DRRIP')
plt.bar(index + 2 * bar_width, ship_values, bar_width, label='SHiP')
plt.bar(index + 3 * bar_width, hawkeye_values, bar_width, label='Hawkeye')
plt.bar(index + 4 * bar_width, rlr_values, bar_width, label='RLR')

plt.xlabel('Trace File')
plt.ylabel('MPKI (lower is better)')
plt.xticks(index + bar_width, trace_names, fontsize=15, rotation=90)
plt.legend()

plt.show()
# plt.savefig("images/plots/mpki-plots.png")