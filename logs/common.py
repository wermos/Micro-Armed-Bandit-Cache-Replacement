import os
import re
import subprocess
from multiprocessing import Pool
from functools import partial
from tqdm import tqdm

def get_trace_names():
    with open("memory_intensive_traces.txt", "r") as f:
        traces = f.read().splitlines()

    trace_names = [re.sub(".trace.xz", "", string) for string in traces]
    return trace_names

# Function to process each trace file
def process_trace(trace_filename, output_dir):
    command = f"./scripts/run_champsim.sh ./traces/{trace_filename}.trace.xz > ./logs/{output_dir}/{trace_filename}.txt"

    # Execute the command using subprocess
    subprocess.run(command, shell=True)

def generate_trace_logs(output_dir):
    # List of all the memory intensive trace files
    trace_filenames = get_trace_names()

    # Number of parallel processes to run
    num_processes = len(os.sched_getaffinity(0))  # Using all available CPU cores
    
    process_trace_private = partial(process_trace, output_dir=output_dir)

    # Use tqdm to display a progress bar
    with Pool(processes=num_processes) as pool:
        with tqdm(total=len(trace_filenames)) as pbar:
            for _ in pool.imap_unordered(process_trace_private, trace_filenames):
                pbar.update()
