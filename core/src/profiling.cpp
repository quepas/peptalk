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

    struct ProfilingInfo {
        int event_set = PAPI_NULL;
        std::vector<std::string> event_names;
        std::vector<int> event_codes;
        int overflow_threshold;
        int num_events = 0;
        bool has_inst_address = false;
        io::PEPWriter pep_writer;
    } global_profiling_info;

    void OverflowCallback(int event_set, void *address, long long overflow_vector, void *context) {
        const auto num_counters = global_profiling_info.num_events;
        long long int counter_values[num_counters];

        int retval;
        if ((retval = PAPI_read(global_profiling_info.event_set, counter_values)) == PAPI_OK) {
            if (global_profiling_info.has_inst_address) {
                global_profiling_info.pep_writer.WriteObservation(counter_values, num_counters, reinterpret_cast<peptalk::io::inst_address_t>(address));
            } else {
                global_profiling_info.pep_writer.WriteObservation(counter_values, num_counters);
            }
        } else {
            cerr << "Failed at reading overflow values" << PAPI_strerror(retval) << std::endl;
        }
    }

    bool
    Init(const std::string &profiling_result_file, const std::string &overflow_event, int overflow_threshold,
         const std::vector<std::string> &measured_events, bool has_inst_address,
         const std::function<void(const std::string &, const std::string &)> &OnErrorOrWarning) {

        // Overflow event is first
        global_profiling_info.overflow_threshold = overflow_threshold;
        global_profiling_info.event_names.push_back(overflow_event);
        global_profiling_info.event_names.insert(global_profiling_info.event_names.end(),
                                                 measured_events.begin(), measured_events.end());

        global_profiling_info.num_events = global_profiling_info.event_names.size();
        global_profiling_info.has_inst_address = has_inst_address;

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

        global_profiling_info.event_codes.resize(global_profiling_info.num_events);
        for (size_t idx = 0; idx < global_profiling_info.num_events; ++idx) {
            auto event_name = global_profiling_info.event_names[idx];
            if ((PAPI_event_name_to_code(event_name.c_str(), &global_profiling_info.event_codes[idx])) != PAPI_OK) {
                OnErrorOrWarning("Failed to retrieve code for event" + event_name, PAPI_strerror(retval));
                return false;
            }
        }
        if ((retval = PAPI_add_events(global_profiling_info.event_set, global_profiling_info.event_codes.data(), global_profiling_info.num_events)) !=
            PAPI_OK) {
            OnErrorOrWarning("Failed to add performance events to the event set", PAPI_strerror(retval));
            return false;
        }

        global_profiling_info.pep_writer.Open(profiling_result_file, has_inst_address);
        return true;
    }

    bool Start(const std::string &trace_header,
               const std::function<void(const std::string &, const std::string &)> &OnErrorOrWarning) {
        global_profiling_info.pep_writer.StartProfile(trace_header, global_profiling_info.event_names);

        int retval;
        if ((retval = PAPI_overflow(global_profiling_info.event_set, global_profiling_info.event_codes[0],
                                    global_profiling_info.overflow_threshold, 0, OverflowCallback)) != PAPI_OK) {
            OnErrorOrWarning("Failed to start the profiling", PAPI_strerror(retval));
            return false;
        }
        if ((retval = PAPI_start(global_profiling_info.event_set)) != PAPI_OK) {
            OnErrorOrWarning("Failed to start the profiling", PAPI_strerror(retval));
            return false;
        }
        return true;
    }

    bool Stop(const function<void(const string &, const string &)> &OnErrorOrWarning) {
        long long counter_values[global_profiling_info.num_events];
        int retval;

        if ((retval = PAPI_stop(global_profiling_info.event_set, counter_values)) != PAPI_OK) {
            OnErrorOrWarning("Failed to stop the profiling", PAPI_strerror(retval));
            return false;
        }
        // Read counters for the very last time
        if (global_profiling_info.has_inst_address) {
            // We are out of the sampling mechanism; thus, the address of a sampled instruction is unknown
            // However, the PEP format still requires some value
            global_profiling_info.pep_writer.WriteObservation(counter_values, global_profiling_info.num_events, 0);
        } else {
            global_profiling_info.pep_writer.WriteObservation(counter_values, global_profiling_info.num_events);
        }

        if ((retval = PAPI_reset(global_profiling_info.event_set)) != PAPI_OK) {
            OnErrorOrWarning("Failed to reset the counters", PAPI_strerror(retval));
            return false;
        }
        // Detach the overflow callback and reset the overflow sampling
        if ((retval = PAPI_overflow(global_profiling_info.event_set, global_profiling_info.event_codes[0],
                                    0, 0, OverflowCallback)) != PAPI_OK) {
            OnErrorOrWarning("Failed to reset the overflow profiling", PAPI_strerror(retval));
            return false;
        }
        global_profiling_info.pep_writer.FinishProfile();
        return true;
    }

    bool Close(error_callback_type &OnErrorOrWarning) {
        int retval;
        if ((retval = PAPI_cleanup_eventset(global_profiling_info.event_set)) != PAPI_OK) {
            OnErrorOrWarning("Failed to cleanup the event set", PAPI_strerror(retval));
            return false;
        }
        if ((retval = PAPI_destroy_eventset(&global_profiling_info.event_set)) != PAPI_OK) {
            OnErrorOrWarning("Failed to destroy the event set", PAPI_strerror(retval));
            return false;
        }
        PAPI_shutdown();
        global_profiling_info.pep_writer.Close();
        return true;
    }

}