import re
import os
import subprocess
from multiprocessing import Pool

def get_trace_names():
    with open("memory_intensive_traces.txt", "r") as f:
        traces = f.read().splitlines()
    
    trace_names = [re.sub(".trace.xz", "", string) for string in traces]
    return trace_names

# Function to process each trace file
def process_trace(trace_filename):
    command = f"./scripts/run_champsim ./traces/{trace_file}.trace.xz > ./logs/lru/{trace_filename}.txt"
    
    # Execute the command using subprocess
    subprocess.run(command, shell=True)

if __name__ == "__main__":
    # List of all the memory intensive trace files
    trace_filenames = get_trace_names()

    # Number of parallel processes to run
    num_processes = len(os.sched_getaffinity(0))  # Using all available CPU cores

    # Use a multiprocessing Pool to parallelize the execution
    with Pool(processes=num_processes) as pool:
        pool.map(process_trace, trace_files)
