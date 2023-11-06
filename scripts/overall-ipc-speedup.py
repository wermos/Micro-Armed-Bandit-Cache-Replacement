import os
import numpy as np

# Path to the main directory
main_directory = 'logs'

# Define dictionaries to store data for each subdirectory
lru_data = {}
rlr_data = {}

# Iterate over subdirectories
for subdir in ['lru', 'rlr']:
    subdir_path = os.path.join(main_directory, subdir)
    files = os.listdir(subdir_path)

    # Extracting data from each file
    for file in files:
        file_path = os.path.join(subdir_path, file)

        # Read the specific line and extract the IPC
        with open(file_path, 'r') as f:
            data = f.read().splitlines()[25]
            
            ipc = data.split()[4]

            # Storing the number in the respective dictionary based on the subdirectory
            if subdir == 'lru':
                lru_data[file[:-4]] = float(ipc)
            elif subdir == 'rlr':
                rlr_data[file[:-4]] = float(ipc)

lru_values = list(lru_data.values())
rlr_values = list(rlr_data.values())

product = 1

for i in range(len(lru_values)):
    product *= rlr_values[i] / lru_values[i]

print(f"Overall IPC Speedup = {product ** (1 / len(lru_values))}")