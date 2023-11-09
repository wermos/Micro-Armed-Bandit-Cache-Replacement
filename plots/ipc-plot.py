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
lru_data = {}
drrip_data = {}
ship_data = {}
hawkeye_data = {}
rlr_data = {}

# Iterate over subdirectories
for subdir in ['lru', 'drrip', 'ship', 'hawkeye', 'rlr']:
    subdir_path = os.path.join(main_directory, subdir)
    files = os.listdir(subdir_path)

    # Extracting data from each file
    for file in files:
        file_path = os.path.join(subdir_path, file)

        # Read the specific line and extract the IPC
        with open(file_path, 'r') as f:
            if not subdir == 'hawkeye':
                data = f.read().splitlines()[25]
                
                ipc = data.split()[4]

                # Storing the number in the respective dictionary based on the subdirectory
                if subdir == 'lru':
                    lru_data[file[:-4]] = float(ipc)
                elif subdir == 'drrip':
                    drrip_data[file[:-4]] = float(ipc)
                elif subdir == 'ship':
                    ship_data[file[:-4]] = float(ipc)
                elif subdir == 'rlr':
                    rlr_data[file[:-4]] = float(ipc)
            else:
                data = f.read().splitlines()[26]
                
                ipc = data.split()[4]
                
                hawkeye_data[file[:-4]] = float(ipc)

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
plt.bar(index + 2 * bar_width, ship_values, bar_width, label='SHIP')
plt.bar(index + 3 * bar_width, hawkeye_values, bar_width, label='Hawkeye')
plt.bar(index + 4 * bar_width, rlr_values, bar_width, label='RLR')

plt.xlabel('Trace File')
plt.ylabel('IPC')
plt.xticks(index + bar_width, trace_names, fontsize=15, rotation=90)
plt.legend()

plt.show()
# plt.savefig("images/ipc-plots.png")