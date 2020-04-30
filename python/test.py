import peptalk

peptalk.timing_init("execution_time.csv", ["test"], 1)
peptalk.timing_start(["no_instructions"])
peptalk.timing_stop()
peptalk.timing_close()

peptalk.counting_init("performance_counters.csv", ["test"], ["INST_RETIRED:ANY_P", "CPU_CLK_UNHALTED:THREAD_P"], 1)
peptalk.counting_start(["no_instructions"])
peptalk.counting_stop()
peptalk.counting_close()

peptalk.profiling_init("performance_profiles.pep", "INST_RETIRED:ANY_P", 2000, ["CPU_CLK_UNHALTED:THREAD_P"], True)
peptalk.profiling_start("no_instructions")
peptalk.profiling_stop()
peptalk.profiling_close()