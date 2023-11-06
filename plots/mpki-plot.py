import os
import numpy as np
import matplotlib
import matplotlib.pyplot as plt

font = {"family" : "Ubuntu",
        "size"   : 22}
matplotlib.rc("font", **font)

# Path to the main directory
main_directory = 'logs'

# Define dictionaries to store data for each subdirectory
ship_data = {}
drrip_data = {}
lru_data = {}
rlr_data = {}

# Iterate over subdirectories
# for subdir in ['lru', 'rlr', 'ship', 'drrip']:
for subdir in ['lru', 'rlr']:
    subdir_path = os.path.join(main_directory, subdir)
    files = os.listdir(subdir_path)

    # Extracting data from each file
    for file in files:
        file_path = os.path.join(subdir_path, file)

        # Read the specific line and extract the number of misses and calculate the MPKI
        with open(file_path, 'r') as f:
            data = f.read().splitlines()[35]
            
            misses = data.split()[-1]

            # Storing the number in the respective dictionary based on the subdirectory
            if subdir == 'ship':
                ship_data[file[:-4]] = (int(misses) / 25_000_000) * 1_000
            elif subdir == 'drrip':
                drrip_data[file[:-4]] = (int(misses) / 25_000_000) * 1_000
            elif subdir == 'lru':
                lru_data[file[:-4]] = (int(misses) / 25_000_000) * 1_000
            elif subdir == 'rlr':
                rlr_data[file[:-4]] = (int(misses) / 25_000_000) * 1_000

# Extracting data for plotting
trace_names = list(lru_data.keys())
# ship_values = list(ship_data.values())
# drrip_values = list(drrip_data.values())
lru_values = list(lru_data.values())
rlr_values = list(rlr_data.values())

fig, ax = plt.subplots(layout="constrained", figsize=(16, 9))

# # Create a bar plot
bar_width = 0.2
index = np.arange(len(trace_names))

# plt.bar(index, ship_values, bar_width, label='SHIP')
# plt.bar(index + bar_width, drrip_values, bar_width, label='DRRIP')
plt.bar(index + 2 * bar_width, lru_values, bar_width, label='LRU')
plt.bar(index + 3 * bar_width, rlr_values, bar_width, label='RLR')

plt.xlabel('Trace File')
plt.ylabel('MPKI (lower is better)')
plt.xticks(index + bar_width, trace_names, fontsize=15, rotation=90)
plt.legend()

plt.show()
# plt.savefig("images/plots/mpki-plots.png")