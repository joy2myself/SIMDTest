#!/usr/bin/python3
import json

def parse_run_name(run_name):
    start = run_name.index("<") + 1
    end = run_name.rindex(">")
    return run_name[start:end].split("_t_v_")[0]

def process_json(filename, N=1, error_rate=0.1):
    with open(filename) as f:
        data = json.load(f)
        benchmarks = data['benchmarks']

    min_cpu_times = []
    min_run_names = []
    min_time_units = []
    for benchmark in benchmarks:
        if benchmark['aggregate_name'] == 'median':
            if not min_cpu_times or benchmark['cpu_time'] <= min_cpu_times[-1] * (1 + error_rate):
                min_cpu_times.append(benchmark['cpu_time'])
                min_run_names.append(benchmark['run_name'])
                min_time_units.append(benchmark['time_unit'])
                if len(min_cpu_times) > N:
                    min_cpu_times.pop()
                    min_run_names.pop()
                    min_time_units.pop()

    for i in range(min(N, len(min_cpu_times))):
        print(f'SIMDBackendType: {parse_run_name(min_run_names[i])}')
        print(f'CPU Time: {min_cpu_times[i]} {min_time_units[i]}\n')

process_json('benchmark_executable.json', N=10, error_rate=0.1)

