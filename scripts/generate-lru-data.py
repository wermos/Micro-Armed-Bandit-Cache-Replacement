import os
import subprocess
from multiprocessing import Pool

def get_traces():
    with open("memory_intensive_traces.txt", "r") as f:
        traces = f.read().splitlines()
    return traces

# Function to process each trace file
def process_trace(trace_file):
    command = f"./scripts/run_champsim {trace_file} > ./logs/lru/{trace_file}.txt"
    
    # Execute the command using subprocess
    subprocess.run(command, shell=True)

if __name__ == "__main__":
    # List all the memory intensive trace files
    trace_files = get_traces()

    # Number of parallel processes to run
    num_processes = len(os.sched_getaffinity(0))  # Using all available CPU cores

    # Use a multiprocessing Pool to parallelize the execution
    with Pool(processes=num_processes) as pool:
        pool.map(process_trace, trace_files)
