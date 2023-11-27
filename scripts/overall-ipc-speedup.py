import os
import numpy as np

# Path to the main directory
main_directory = 'logs'

# Define dictionaries to store data for each subdirectory
lru_data = {}
drrip_data = {}
ship_data = {}
hawkeye_data = {}
rlr_data = {}
mab_data = {}
srrip_data = {}

# Iterate over subdirectories
for subdir in ['lru', 'drrip', 'ship', 'hawkeye', 'rlr', 'micro-armed-bandit', 'srrip']:
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
                elif subdir == 'micro-armed-bandit':
                    mab_data[file[:-4]] = float(ipc)
                elif subdir == 'srrip':
                    srrip_data[file[:-4]] = float(ipc)
            else:
                data = f.read().splitlines()[26]

                ipc = data.split()[4]

                hawkeye_data[file[:-4]] = float(ipc)

lru_values = list(lru_data.values())
drrip_values = list(drrip_data.values())
ship_values = list(ship_data.values())
hawkeye_values = list(hawkeye_data.values())
srrip_values = list(srrip_data.values())
rlr_values = list(rlr_data.values())
mab_values = list(mab_data.values())

product = [1] * 6 # one for each of the non-LRU policies
# product = 1

# for j, _ in enumerate(['drrip', 'ship', 'hawkeye', 'rlr']):
#     for i in range(len(lru_values)):
#         product[j] *= rlr_values[i] / lru_values[i]

for i in range(len(lru_values)):
    product[0] *= drrip_values[i] / lru_values[i]
    product[1] *= ship_values[i] / lru_values[i]
    product[2] *= hawkeye_values[i] / lru_values[i]
    product[3] *= srrip_values[i] / lru_values[i]
    product[4] *= rlr_values[i] / lru_values[i]
    product[5] *= mab_values[i] / lru_values[i]

print(f"Overall IPC Speedup over LRU:")
print(f"DRRIP: {product[0] ** (1 / len(lru_values))}")
print(f"SHIP: {product[1] ** (1 / len(lru_values))}")
print(f"Hawkeye: {product[2] ** (1 / len(lru_values))}")
print(f"SRRIP: {product[3] ** (1 / len(lru_values))}")
print(f"RLR : {product[4] ** (1 / len(lru_values))}")
print(f"Micro-armed bandit: {product[5] ** (1 / len(lru_values))}")