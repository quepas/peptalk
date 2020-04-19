//
// Created by quepas on 15/04/2020.
//

#include "counting.hpp"

#include <papi.h>
#include <experimental/filesystem>

using std::string;
using std::to_string;

namespace peptalk::counting {

    const char DELIMITER = ',';

    struct ProfilingInfo {
        int event_set = PAPI_NULL;
        std::vector<std::string> performance_events;
        std::vector<std::string> parameters;
        std::vector<std::string> to_save;
        std::vector<long long int> measurements;
        int num_events = 0;
        std::string result_file;
        FILE *result_fd = nullptr;
    } global_counting_info;

    bool Init(const std::string &result_file,
              const std::vector<std::string> &parameters_names,
              const std::vector<std::string> &performance_events,
              unsigned int num_measurements,
              error_callback_type &OnErrorOrWarning) {
        global_counting_info.result_file = result_file;
        global_counting_info.performance_events = performance_events;
        global_counting_info.num_events = global_counting_info.performance_events.size();
        int retval;
        if (!PAPI_is_initialized()) {
            retval = PAPI_library_init(PAPI_VER_CURRENT);
            if (retval != PAPI_VER_CURRENT) {
                OnErrorOrWarning("PAPI initialisation failed", PAPI_strerror(retval));
                return false;
            }
        }
        global_counting_info.event_set = PAPI_NULL;
        if ((retval = PAPI_create_eventset(&global_counting_info.event_set)) != PAPI_OK) {
            OnErrorOrWarning("Failed to create an event set", PAPI_strerror(retval));
            return false;
        }
        int codes[global_counting_info.num_events];
        for (size_t idx = 0; idx < global_counting_info.num_events; ++idx) {
            auto event_name = global_counting_info.performance_events[idx];
            if ((PAPI_event_name_to_code(event_name.c_str(), &codes[idx])) != PAPI_OK) {
                OnErrorOrWarning("Failed to retrieve code for event: " + event_name, PAPI_strerror(retval));
                return false;
            }
        }
        if ((retval = PAPI_add_events(global_counting_info.event_set, codes, global_counting_info.num_events)) !=
            PAPI_OK) {
            OnErrorOrWarning("Failed to add performance events to the event set.", PAPI_strerror(retval));
            return false;
        }
        auto result_file_exists = std::experimental::filesystem::exists(result_file);
        global_counting_info.measurements.reserve(num_measurements * global_counting_info.num_events);
        global_counting_info.result_fd = fopen(global_counting_info.result_file.c_str(), "a");
        if (!result_file_exists) {
            string header;
            for (auto &name : parameters_names) {
                header += name;
                header += DELIMITER;
            }
            header += "metrics,value\n";
            fputs(header.c_str(), global_counting_info.result_fd);
        }
        return true;
    }

    bool Start(const std::vector<std::string> &parameters, error_callback_type &OnErrorOrWarning) {
        global_counting_info.parameters = parameters;
        int retval;
        if ((retval = PAPI_start(global_counting_info.event_set)) != PAPI_OK) {
            OnErrorOrWarning("Failed to start the profiling.", PAPI_strerror(retval));
            return false;
        }
        return true;
    }

    bool Stop(error_callback_type &OnErrorOrWarning) {
        long long values[global_counting_info.num_events];
        int retval;

        if ((retval = PAPI_stop(global_counting_info.event_set, values)) != PAPI_OK) {
            OnErrorOrWarning("Failed to start the profiling.", PAPI_strerror(retval));
            return false;
        }
        if ((retval = PAPI_reset(global_counting_info.event_set)) != PAPI_OK) {
            OnErrorOrWarning("Failed to reset the counters.", PAPI_strerror(retval));
            return false;
        }
        for (int idx = 0; idx < global_counting_info.num_events; ++idx) {
            global_counting_info.measurements.push_back(values[idx]);
        }
        // Combine parameters, add measurement, store for saving
        string line;
        auto parameters = global_counting_info.parameters;
        for (const auto &parameter : parameters) {
            line += parameter;
            line += DELIMITER;
        }
        global_counting_info.to_save.push_back(line);
        return true;
    }

    bool Close(error_callback_type &OnErrorOrWarning) {
        int retval;
        if ((retval = PAPI_cleanup_eventset(global_counting_info.event_set)) != PAPI_OK) {
            OnErrorOrWarning("Failed to cleanup the event set.", PAPI_strerror(retval));
            return false;
        }
        if ((retval = PAPI_destroy_eventset(&global_counting_info.event_set)) != PAPI_OK) {
            OnErrorOrWarning("Failed to destroy the event set.", PAPI_strerror(retval));
            return false;
        }
        PAPI_shutdown();

        size_t measurement_idx = 0;
        for (size_t idx = 0; idx < global_counting_info.to_save.size(); ++idx) {
            string line_parameters = global_counting_info.to_save[idx];
            for (size_t m_idx = 0; m_idx < global_counting_info.num_events; ++m_idx) {
                string line_to_save = line_parameters
                                      + global_counting_info.performance_events[m_idx]
                                      + ","
                                      + to_string(global_counting_info.measurements[measurement_idx++])
                                      + "\n";
                fputs(line_to_save.c_str(), global_counting_info.result_fd);
            }
        }
        fclose(global_counting_info.result_fd);
        return true;
    }

}