//
// Created by quepas on 14/02/2020.
//

#include "profiling.hpp"
#include "PEPWriter.hpp"

#include <iostream>
#include <papi.h>

using std::cerr;
using std::endl;
using std::function;
using std::string;
using std::to_string;
using std::vector;

namespace peptalk::profiling {

    const string INSTRUCTION_ADDRESS_NAME = "INST_ADDRESS";

    struct ProfilingInfo {
        int event_set = PAPI_NULL;
        std::vector<std::string> performance_events;
        unsigned long int trace_num_measurements;
        int num_events = 0;
        bool include_instruction_address = false;
        io::PEPWriter pep_writer;
    } global_profiling_info;

    void ReadMeasurements(void *address, bool last_measurement = false) {
        auto has_address = global_profiling_info.include_instruction_address && !last_measurement;
        auto num_counters = global_profiling_info.num_events + (has_address ? 1 : 0);

        long long int counter_values[num_counters];
        if (has_address) {
            counter_values[num_counters] = reinterpret_cast<std::intptr_t>(address);
        }
        int retval;
        if ((retval = PAPI_read(global_profiling_info.event_set, counter_values)) == PAPI_OK) {
            global_profiling_info.pep_writer.WriteMeasurements(counter_values, num_counters);
            global_profiling_info.trace_num_measurements += num_counters;
        } else {
            cerr << "Failed at reading overflow values. Error: " << PAPI_strerror(retval) << std::endl;
        }
    }

    void OverflowCallback(int event_set, void *address, long long overflow_vector, void *context) {
        ReadMeasurements(address);
    }

    bool
    Init(const std::string &profiling_result_file, const std::string &overflow_event, int overflow_threshold,
         const std::vector<std::string> &measured_events, bool include_instruction_address,
         const std::function<void(const std::string &, const std::string &)> &OnErrorOrWarning) {

        // Overflow event is first
        global_profiling_info.performance_events.push_back(overflow_event);
        global_profiling_info.performance_events.insert(global_profiling_info.performance_events.end(),
                                                        measured_events.begin(), measured_events.end());

        global_profiling_info.num_events = global_profiling_info.performance_events.size();
        global_profiling_info.include_instruction_address = include_instruction_address;

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

        global_profiling_info.pep_writer.Open(profiling_result_file);
        return true;
    }

    bool Start(const std::string &trace_header,
               const std::function<void(const std::string &, const std::string &)> &OnErrorOrWarning) {
        global_profiling_info.trace_num_measurements = 0;
        auto performance_event_names = global_profiling_info.performance_events;
        if (global_profiling_info.include_instruction_address) {
            performance_event_names.emplace_back(INSTRUCTION_ADDRESS_NAME);
        }
        global_profiling_info.pep_writer.StartProfile(trace_header, performance_event_names);

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
        // Read counters for the very last time
        ReadMeasurements(nullptr, true);

        if ((retval = PAPI_reset(global_profiling_info.event_set)) != PAPI_OK) {
            OnErrorOrWarning("Failed to reset the counters.", PAPI_strerror(retval));
            return false;
        }
        global_profiling_info.pep_writer.FinishProfile();
        return true;
    }

    bool Close(error_callback_type &OnErrorOrWarning) {
        int retval;
        if ((retval = PAPI_cleanup_eventset(global_profiling_info.event_set)) != PAPI_OK) {
            OnErrorOrWarning("Failed to cleanup the event set.", PAPI_strerror(retval));
            return false;
        }
        if ((retval = PAPI_destroy_eventset(&global_profiling_info.event_set)) != PAPI_OK) {
            OnErrorOrWarning("Failed to destroy the event set.", PAPI_strerror(retval));
            return false;
        }
        PAPI_shutdown();
        global_profiling_info.pep_writer.Close();
        return true;
    }

}