import os
from concurrent.futures import ProcessPoolExecutor

from common import generate_trace_logs
from util import modify_mab_period, build_champsim

if __name__ == "__main__":
    periods = [75, 200, 500]
    
    with ProcessPoolExecutor(max_workers=2) as executor:
        for period in periods:
            modify_mab_period(period)
            build_champsim()
            
            dirname = "./logs/tuning-period-" + str(period)
            
            if not os.path.exists(dirname):
                os.mkdir(dirname)
            
            future = executor.submit(generate_trace_logs, dirname)