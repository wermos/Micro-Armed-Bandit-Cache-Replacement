import os
import subprocess
from multiprocessing import Pool
from tqdm import tqdm

def process_trace(trace_file):
    # Run the specific command and capture output in a text file
    output_file = f"{os.path.splitext(trace_file)[0]}.txt"
    
    command = f"./scripts/run_champsim.sh ./gap/{trace_file} > {output_file}"
    subprocess.run(command, shell=True)

    # Run analysis.py on the generated text file
    analysis_command = f"python ./scripts/analysis.py {output_file}"
    result = subprocess.run(analysis_command, shell=True, capture_output=True, text=True)
    if result.stdout == "OK\n":
        # The trailing `\n` is simply due to how `subprocess.run` works
        with open('trace-names/gap_memory_intensive_traces.txt', 'a') as file:
            file.write(trace_file)
            file.write("\n")
    
    # Delete the log file after we're done
    delete_command = f"rm {output_file}"
    subprocess.run(delete_command, shell=True)

if __name__ == "__main__":
    # List of all the trace files
    trace_filenames = os.listdir("gap")

    # Number of parallel processes to run
    num_processes = len(os.sched_getaffinity(0))  # Using all available CPU cores
    
    custom_format = "{desc}: {percentage:3.0f}%|{bar}| {n_fmt}/{total_fmt} [{elapsed}<{remaining}, {rate_inv_fmt}{postfix}]"

    # Use tqdm to display a progress bar
    with Pool(processes=num_processes) as pool:
        with tqdm(total=len(trace_filenames), desc="Processing trace files", unit="trace", bar_format=custom_format) as pbar:
            for _ in pool.imap_unordered(process_trace, trace_filenames):
                pbar.update()
