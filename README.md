# test-tool
PC load tool and resource acquisition application

## memget
Memory load tool

Do not talk about securing memory for specified arguments
It can be used to check the operation of programs in high load state.

## cpuget
CPU load tool

A tool to crush the resources of the CPU. By specifying the number of cores and the number of logical threads, you can check the operation of the program in the absence of computational resources.

##### Runnninng
$ make

$ ./mem_get get_memory

$ ./cpu_get get_cpu_num

$ make clean

