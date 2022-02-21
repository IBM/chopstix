# perf-invok
`perf`-like tool to measure the performance on a per-invocation basis (aside from the performance of the overall, complete, execution).
Note, however, that although its intent is to measure performance on an invocatoin level, because the input parameters are just two memory addresses, this tool can measure the performance of a region of code whenever it is executed.

Originally it was targeted at RiscV, meaning it had to use processor-specific PMU/HPM code. More recently, it has been updated to support any platform which, in turn, supports Linux's `perf` subsystem.

## Usage

### Profiling a code region or a function

```
perf-invok -begin <start_address> -end <end_address> [-o <output_file>] [-max <count>] COMMAND
```

- `-begin`: begin address of the code region or function to profile, in hexadecimal and without `0x`
- `-end`: end address of the code region or function to profile, in hexadecimal and without `0x`
- `-o`: output the csv data into this file
- `-max`: profile, at most, `count` executions of the profiled region

### Profiling the overall execution of a program

```
perf-invok [-o <output_file>] [-timeout <time>] COMMAND
```

- `-o`: output the csv data into this file
- `-timeout`: profile for, at most, `time` seconds

## Example output

```
> perf-invok -begin 401126 -end 40118f ./example
Executing ./example 
Measuring performance counters from 0x401126 to 0x40118f (max. samples: 4294967295).
Begin...
End
Cycles, Time Elapsed (us), Retired Instructions, Retired Memory Instructions, Data Cache Misses, Instructions Per Cycle, Miss Percentage
98788598, 36884, 200000722, 130001947, 1835317, 2.024532, 1.411761
9299681, 2986, 20000017, 13000033, 155246, 2.150613, 1.194197
94285350, 30490, 200000027, 130000191, 1842946, 2.121221, 1.417649
9271890, 2963, 20000016, 13000026, 155294, 2.157059, 1.194567
93638483, 30252, 200000029, 130000200, 1842909, 2.135875, 1.417620
9300606, 2937, 20000015, 13000014, 155232, 2.150399, 1.194091
93758861, 30246, 200000028, 130000227, 1843042, 2.133132, 1.417722
9274297, 2964, 20000016, 13000036, 155382, 2.156499, 1.195243
93780481, 29965, 200000027, 130000184, 1843010, 2.132640, 1.4
```
