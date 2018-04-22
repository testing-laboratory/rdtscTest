# RDTSC testing tool

This tool is designed to test x86 rdtsc command speed and to test speed for proof of concept when rdtsc is read in one thread and result is passed to another thread via memory.

## Experiment

Used CPU: Core i7-4700MQ 2.4GHz (8 core, mobile laptop)
Compilation mode: Release, x64

### Experiment results

read memory = 0.16ns (~0.38 cycle)
read memory while it is written = 3.4ns (~8 cycles)

single rdtsc = 7.6ns (~18 cycles)
rdtsc + write to memory = 7.6ns (~18 cycles)
rdtsc and write memory while it is read = 7.6ns (~18 cycles)

#### Results conclusion

Using rdtsc will have average error: +7 ns
Using memory read will have average error: +3 ns +- 7 ns

### Original program console output
~~~~
HELLO
=====================================================================
Begin of experiment readTSC(2e+08)...
End of experiment readTSC(2e+08)
readTSC time taken: totally: 1533ms; average cycle time: 7ns or 7665ps
=====================================================================
Begin of experiment readTSCtoMem(1e+08)...
End of experiment readTSCtoMem(1e+08)
readTSCtoMem time taken: totally: 762ms; average cycle time: 7ns or 7620ps
=====================================================================
Begin of experiment readMemory(1e+10)...
End of experiment readMemory(1e+10)
readMemory time taken: totally: 1606ms; average cycle time: 0ns or 160ps
=====================================================================
DO THE SAME IN PARALLEL; Reading TSC is longer than Memory (i.e. Memory time has meaning)...
=====================================================================
=====================================================================
Begin of experiment readTSCtoMem(5e+08)...
=====================================================================
Begin of experiment readMemory(1e+09)...
End of experiment readMemory(1e+09)
readMemory time taken: totally: 3450ms; average cycle time: 3ns or 3450ps
End of experiment readTSCtoMem(5e+08)
readTSCtoMem time taken: totally: 3840ms; average cycle time: 7ns or 7680ps
=====================================================================
DO THE SAME IN PARALLEL; Reading Memory is longer than TSC (i.e. TSC time has meaning)...
=====================================================================
=====================================================================
Begin of experiment readMemory(2e+10)...
=====================================================================
Begin of experiment readTSCtoMem(1e+08)...
End of experiment readTSCtoMem(1e+08)
readTSCtoMem time taken: totally: 768ms; average cycle time: 7ns or 7680ps
End of experiment readMemory(2e+10)
readMemory time taken: totally: 3872ms; average cycle time: 0ns or 193ps
=====================================================================
END
~~~~
