# Register file criticality on embedded microprocessor reliability (Fault Injector Register File)
 
 Related Articles and Dissertation:
 
 Register file criticality on embedded microprocessor reliability
 
 https://ieeexplore.ieee.org/document/8093143
 
 We investigate the criticality of register file and compiler optimization on ARM-based SoCs reliability. The analysis performed using a homemade interruption-based efficient fault injector is correlated with heavy ions experiment results.
 
 
 Register File Criticality and Compiler Optimization Effects on Embedded Microprocessor Reliability
 
  https://ieeexplore.ieee.org/document/7930516
 
 In this paper, we investigate the impact of register file errors in modern embedded microprocessors reliability through fault-injection and heavy-ion experiments. Additionally, we evaluate how different levels of compiler optimization modify the usage and failure probability of a processor register file. We select six representative benchmarks, each one compiled with three different levels of compiler optimization. We performed exhaustive fault-injection campaigns to measure the register's architectural vulnerability factor of each code and configuration, identifying the registers that are more likely to generate silent data corruption or single event functional interruption. Moreover, we correlate the observed reliability variations with register file utilization. Finally, we irradiated with heavy ions two of the selected benchmarks compiled with two levels of optimization, and correlated experimental results with fault-injection analysis.
 
 
Dissertation:


The Effects of the Compiler Optimizations in Embedded Processors Reliability

https://lume.ufrgs.br/handle/10183/169248

The recent advances in the embedded processors increase the compilers complexity, and
the usage of heterogeneous resources such as Field Programmable Gate Array (FPGA)
and Graphics Processing Unit (GPU) integrated with the processors. Additionally, the
increase in the usage of Commercial off-the-shelf (COTS) instead of radiation hardened
chips in safety critical applications occurs because the COTS can be more flexible, inexpensive, have a fast time-to market and a lower power consumption. However, even
with these advantages, it is still necessary to guarantee a high reliability in a system that
uses a COTS for safety critical applications because they are susceptible to failures. Additionally, in the case of real time applications, the time requirements also need to be
respected. As a case of study, this work uses the Zynq which is a COTS device classified
as an All Programmable System-on-Chip (APSOC) and has an ARM Cortex-A9 as the
embedded processor. In this research, the impact of faults that affect the register file in the
embedded processors reliability was investigated. For that, fault-injection and heavy-ion
radiation experiments were performed. Moreover, an evaluation of how the different levels of compiler optimization modify the usage and the failure probability of a processor
register file. A set of six representative benchmarks, each one compiled with three different levels of compiler optimization. Exhaustive fault injection campaigns were performed
to measure the registers Architectural Vulnerability Factor (AVF) of each code and configuration, identifying the registers that are more likely to generate Silent Data Corruption
(SDC) or Single Event Functional Interruption (SEFI). Moreover, the observed reliability
variations with register file utilization were correlated. Finally, two of the selected benchmarks, each one compiled with two different levels of optimization were irradiated in the
heavy ions experiments. The results show that the best performance, the minor register
file usage, or the lowest AVF does not always bring the highest Mean Workload Between
Failures (MWBF). As an example, in the Matrix Multiplication (MxM) application, the
best performance is achieved in the highest compiler optimization. However, in the fault
injection, the higher reliability is obtained in the lower compiler optimization which has,
the lower AVFs and the lower register file usage. Results also show that the impact of optimizations is strongly related to the executed algorithm and how the compiler optimizes
them

 
 
