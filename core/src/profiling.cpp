//
// Created by quepas on 14/02/2020.
//

#include "profiling.hpp"

#include <iostream>
#include <papi.h>

using std::cerr;
using std::endl;
using std::function;
using std::string;
using std::to_string;
using std::vector;

namespace peptalk::profiling {

    struct ProfilingInfo {
        int event_set = PAPI_NULL;
        std::vector<std::string> performance_events;
        int num_events = 0;
        bool include_instruction_address = false;
        std::string profiling_result_file;
        FILE *pep_fd = nullptr;
    } global_profiling_info;

    void OverflowCallback(int event_set, void *address, long long overflow_vector, void *context) {
        long long counter_values[global_profiling_info.num_events];
        int retval;
        if ((retval = PAPI_read(global_profiling_info.event_set, counter_values)) == PAPI_OK) {
            string values_str;
            for (int idx = 0; idx < global_profiling_info.num_events; ++idx) {
                values_str.append(to_string(counter_values[idx]));
                if (idx + 1 < global_profiling_info.num_events) {
                    values_str.append(",");
                }
            }
            if (global_profiling_info.include_instruction_address) {
                values_str.append(",");
                values_str.append(to_string(reinterpret_cast<std::intptr_t>(address)));
            }
            values_str.append("\n");
            fputs(values_str.c_str(), global_profiling_info.pep_fd);
        } else {
            cerr << "Failed at reading overflow values. Error: " << PAPI_strerror(retval) << std::endl;
        }
    }

    bool
    Init(const std::string &overflow_event, int overflow_threshold, const std::vector<std::string> &measured_events,
         bool include_instruction_address, const std::string &profiling_result_file,
         const std::function<void(const std::string &, const std::string &)> &OnErrorOrWarning) {

        // Overflow event is first
        global_profiling_info.performance_events.push_back(overflow_event);
        global_profiling_info.performance_events.insert(global_profiling_info.performance_events.end(),
                                                        measured_events.begin(), measured_events.end());

        global_profiling_info.num_events = global_profiling_info.performance_events.size();
        global_profiling_info.include_instruction_address = include_instruction_address;
        global_profiling_info.profiling_result_file = profiling_result_file;

        int retval;
        if (!PAPI_is_initialized()) {
            retval = PAPI_library_init(PAPI_VER_CURRENT);
            if (retval != PAPI_VER_CURRENT) {
                OnErrorOrWarning("PAPI initialisation failed", PAPI_strerror(retval));
                return false;
            }
        }
        global_profiling_info.event_set = PAPI_NULL;
        if ((retval = PAPI_create_eventset(&global_profiling_info.event_set)) != PAPI_OK) {
            OnErrorOrWarning("Failed to create an event set", PAPI_strerror(retval));
            return false;
        }

        int codes[global_profiling_info.num_events];
        for (size_t idx = 0; idx < global_profiling_info.num_events; ++idx) {
            auto event_name = global_profiling_info.performance_events[idx];
            if ((PAPI_event_name_to_code(event_name.c_str(), &codes[idx])) != PAPI_OK) {
                OnErrorOrWarning("Failed to retrieve code for event: " + event_name, PAPI_strerror(retval));
                return false;
            }
        }
        if ((retval = PAPI_add_events(global_profiling_info.event_set, codes, global_profiling_info.num_events)) !=
            PAPI_OK) {
            OnErrorOrWarning("Failed to add performance events to the event set.", PAPI_strerror(retval));
            return false;
        }
        if ((retval = PAPI_overflow(global_profiling_info.event_set, codes[0],
                                    overflow_threshold, 0, OverflowCallback)) != PAPI_OK) {
            OnErrorOrWarning("Failed to start the profiling.", PAPI_strerror(retval));
            return false;
        }
        global_profiling_info.pep_fd = fopen(global_profiling_info.profiling_result_file.c_str(), "a");
        return true;
    }

    bool Start(const std::string &trace_header,
               const std::function<void(const std::string &, const std::string &)> &OnErrorOrWarning) {
        fprintf(global_profiling_info.pep_fd, "@trace_start:%s\n", trace_header.c_str());
        fprintf(global_profiling_info.pep_fd, "@perf_events:%s\n", "perf_events");

        int retval;
        if ((retval = PAPI_start(global_profiling_info.event_set)) != PAPI_OK) {
            OnErrorOrWarning("Failed to start the profiling.", PAPI_strerror(retval));
            return false;
        }
        return true;
    }

    bool Stop(const function<void(const string &, const string &)> &OnErrorOrWarning) {
        long long values[global_profiling_info.num_events];
        int retval;

        if ((retval = PAPI_stop(global_profiling_info.event_set, values)) != PAPI_OK) {
            OnErrorOrWarning("Failed to start the profiling.", PAPI_strerror(retval));
            return false;
        }
        if ((retval = PAPI_reset(global_profiling_info.event_set)) != PAPI_OK) {
            OnErrorOrWarning("Failed to reset the counters.", PAPI_strerror(retval));
            return false;
        }
        fputs("@trace_end\n", global_profiling_info.pep_fd);
        fflush(global_profiling_info.pep_fd);
        return true;
    }

}