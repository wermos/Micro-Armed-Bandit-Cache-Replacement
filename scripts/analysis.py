import re
import sys

def calculate_mpki(file):
    with open(file, 'r') as f:
        data = f.read().splitlines()

        # This line has info about LLC hits and misses
        matching = [s for s in data if "LLC TOTAL" in s]

        assert(len(matching) == 1)

        matching = re.sub(' +', ' ', matching[0])

        words = matching.split()

        num_instr = 25_000_000
        miss = int(words[-1])
        # print(words)
        # print(miss)
        mpki = (miss / num_instr) * 1_000
        #print(mpki)
        
        return mpki

if __name__ == "__main__":
    filename = sys.argv[1]
    
    if calculate_mpki(filename) > 1:
        print("OK")
    else:
        print("NOT OK")