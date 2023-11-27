import subprocess

def modify_mab_period(period):
    with open("./ChampSim/inc/ooo_cpu.h", 'r') as file:
        # read a list of lines into data
        data = file.readlines()
    
    with open("./ChampSim/inc/ooo_cpu.h", 'w') as file:
        for lineno, line in enumerate(data):
            if lineno == 47:
                # modify the period to the desired value
                file.write(f'static constexpr std::uint64_t MAB_IPC_UPDATE_PERIOD = {period};\n')
            else:
                file.write(line)

def modify_c_value(c):
    with open("./micro-armed-bandit/orchestrator.hpp", 'r') as file:
        # read a list of lines into data
        data = file.readlines()
    
    with open("./micro-armed-bandit/orchestrator.hpp", 'w') as file:
        for lineno, line in enumerate(data):
            if lineno == 20:
                # modify the period to the desired value
                file.write(f'Orchestrator(std::size_t n) : N{{n}}, c{{{c}}}, gamma{{0.975}}, currentPolicy{{0}}, nextUpdateCycle{{0}} {{\n')
            else:
                file.write(line)

def build_champsim():
    command = "./scripts/build_champsim.sh ./champsim-configs/mab-config.json"

    # Execute the command using subprocess
    subprocess.run(command, shell=True)

if __name__ == "__main__":
    modify_mab_period(500)
    # modify_c_value(10)
