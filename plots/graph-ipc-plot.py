import argparse
import os
import numpy as np
import matplotlib
import matplotlib.pyplot as plt

font = {"family" : "Ubuntu",
        "size"   : 22}
matplotlib.rc("font", **font)

def read_ipc(file, subdir):
    # Read the specific line and extract the IPC
    if not subdir == 'hawkeye':
        data = file.read().splitlines()[25]
    else:
        data = file.read().splitlines()[26]

    ipc = float(data.split()[4])

    return ipc

def parse_data(function):
    # Path to the main directory
    main_directory = 'logs/gap'

    lru_data = {}
    mab_data = {}

    # Iterate over subdirectories
    for subdir in ['lru', 'mab']:
        subdir_path = os.path.join(main_directory, subdir)
        files = os.listdir(subdir_path)

        # Extracting data from each file
        for file in files:
            file_path = os.path.join(subdir_path, file)

            with open(file_path, 'r') as f:
                # Storing the number in the respective dictionary based on the subdirectory
                if subdir == 'lru':
                    lru_data[file[:-4]] = function(f, subdir)
                elif subdir == 'mab':
                    mab_data[file[:-4]] = function(f, subdir)

    return lru_data, mab_data

def parse_ipc():
    return parse_data(read_ipc)

lru_data, mab_data = parse_ipc()

# Extracting data for plotting
trace_names = list(lru_data.keys())

lru_values = list(lru_data.values())
mab_values = list(mab_data.values())

fig, ax = plt.subplots(layout="constrained", figsize=(16, 9))

# Create a bar plot
bar_width = 0.1
index = np.arange(len(trace_names))

plt.bar(index, lru_values, bar_width, label='LRU')
plt.bar(index + bar_width, mab_values, bar_width, label='Micro-Armed Bandit')

plt.xlabel('Trace File')
plt.ylabel('IPC')
plt.xticks(index + bar_width, trace_names, fontsize=15, rotation=90)
plt.legend()


parser = argparse.ArgumentParser()

parser.add_argument('-s', '--save', action='store_true', default=False)

args = parser.parse_args()

if args.save:
    plt.savefig("images/ipc-plot.png")
    plt.savefig("images/ipc-plot.pdf")
else:
    plt.show()