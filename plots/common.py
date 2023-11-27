import os

def read_ipc(file, subdir):
    # Read the specific line and extract the IPC
    if not subdir == 'hawkeye':
        data = file.read().splitlines()[25]
    else:
        data = file.read().splitlines()[26]

    ipc = float(data.split()[4])

    return ipc

def read_mpki(file, subdir):
    # Read the specific line and extract the MPKI
    if not subdir == 'hawkeye':
        data = file.read().splitlines()[35]
    else:
        data = file.read().splitlines()[36]

    misses = data.split()[-1]

    mpki = (int(misses) / 25_000_000) * 1_000

    return mpki

def parse_data(function):
    # Path to the main directory
    main_directory = 'logs'

    lru_data = {}
    drrip_data = {}
    ship_data = {}
    hawkeye_data = {}
    srrip_data = {}
    rlr_data = {}
    mab_data = {}

    # Iterate over subdirectories
    for subdir in ['lru', 'drrip', 'ship', 'hawkeye', 'rlr', 'micro-armed-bandit', 'srrip']:
        subdir_path = os.path.join(main_directory, subdir)
        files = os.listdir(subdir_path)

        # Extracting data from each file
        for file in files:
            file_path = os.path.join(subdir_path, file)

            with open(file_path, 'r') as f:
                # Storing the number in the respective dictionary based on the subdirectory
                if subdir == 'lru':
                    lru_data[file[:-4]] = function(f, subdir)
                elif subdir == 'drrip':
                    drrip_data[file[:-4]] = function(f, subdir)
                elif subdir == 'ship':
                    ship_data[file[:-4]] = function(f, subdir)
                elif subdir == 'rlr':
                    rlr_data[file[:-4]] = function(f, subdir)
                elif subdir == 'hawkeye':
                    hawkeye_data[file[:-4]] = function(f, subdir)
                elif subdir == 'micro-armed-bandit':
                    mab_data[file[:-4]] = function(f, subdir)
                elif subdir == 'srrip':
                    srrip_data[file[:-4]] = function(f, subdir)

    return lru_data, drrip_data, ship_data, hawkeye_data, rlr_data, srrip_data, mab_data

def parse_ipc():
    return parse_data(read_ipc)

def parse_mpki():
    return parse_data(read_mpki)
