import urllib.request
from tqdm import tqdm

def download_files(file_list_path, base_url, download_folder):
    with open(file_list_path, 'r') as file:
        filenames = file.read().splitlines()

    for filename in tqdm(filenames, desc="Downloading traces:"):
        file_url = f'{base_url}/{filename}'
        try:
            urllib.request.urlretrieve(file_url, f'{download_folder}/{filename}')
            print(f'{filename} downloaded successfully.')
        except urllib.error.HTTPError as e:
            print(f'Failed to download {filename}. HTTPError: {e.code}')
        except urllib.error.URLError as e:
            print(f'Failed to download {filename}. URLError: {e.reason}')

file_list_path = './trace-names/spec2017_memory_intensive_traces.txt'
base_url = 'https://dpc3.compas.cs.stonybrook.edu/champsim-traces/speccpu'
download_folder = 'traces'

download_files(file_list_path, base_url, download_folder)
