const peptalk = require('./lib/pep-talk-node')

peptalk.timingInit("execution_time.csv", ["test"], 1);
peptalk.timingStart(["no_instructions"]);
peptalk.timingStop();
peptalk.timingClose();

peptalk.countingInit("performance_counters.csv", ["test"], ["INST_RETIRED:ANY_P", "CPU_CLK_UNHALTED:THREAD_P"], 1);
peptalk.countingStart(["no_instructions"]);
peptalk.countingStop();
peptalk.countingClose();

peptalk.profilingInit("performance_profiles.pep", "INST_RETIRED:ANY_P", 2000, ["CPU_CLK_UNHALTED:THREAD_P"], true);
peptalk.profilingStart("no_instructions");
peptalk.profilingStop();
peptalk.profilingClose();
