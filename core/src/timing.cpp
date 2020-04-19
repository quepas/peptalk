//
// Created by quepas on 13/04/2020.
//

#if defined __cpp_lib_filesystem
    #include <filesystem>
    namespace fs = std::filesystem;
#else
    #include <experimental/filesystem>
    namespace fs = std::experimental::filesystem;
#endif
#include "timing.hpp"

using std::string;
using std::to_string;

namespace peptalk::timing {

    const char DELIMITER = ',';

    struct TimingInfo {
        std::string result_file;
        FILE* result_fd = nullptr;
        GenericTimer* timer = new SystemClockTimer();
        std::vector<std::string> parameters;
        std::vector<std::string> to_save;
        std::vector<double> time_measurements;
    } global_timing_info;

    void Init(const std::string& result_file, const std::vector<std::string>& parameters_names, unsigned int num_measurements, error_callback_type &OnErrorOrWarning) {
        global_timing_info.result_file = result_file;
        global_timing_info.time_measurements.reserve(num_measurements);
        global_timing_info.to_save.reserve(num_measurements);
        auto result_file_exists = fs::exists(result_file);
        global_timing_info.result_fd = fopen(global_timing_info.result_file.c_str(), "a");
        if (!result_file_exists) {
            string header;
            for (auto& name : parameters_names) {
                header += name;
                header += DELIMITER;
            }
            header += "value\n";
            fputs(header.c_str(), global_timing_info.result_fd);
        }
    }

    void Start(const std::vector<std::string>& parameters, error_callback_type &OnErrorOrWarning) {
        global_timing_info.parameters = parameters;
        global_timing_info.timer->reset();
    }

    void Stop(error_callback_type &OnErrorOrWarning) {
        global_timing_info.time_measurements.push_back(global_timing_info.timer->elapsed());
        // Combine parameters, add measurement, store for saving
        string line;
        auto parameters = global_timing_info.parameters;
        for (const auto & parameter : parameters) {
            line += parameter;
            line += DELIMITER;
        }
        global_timing_info.to_save.push_back(line);
    }

    void Close(error_callback_type &OnErrorOrWarning) {
        // Save time measurements
        for (int idx = 0; idx < global_timing_info.to_save.size(); ++idx) {
            string line_to_save = global_timing_info.to_save[idx]
                    + to_string(global_timing_info.time_measurements[idx])
                    + "\n";
            fputs(line_to_save.c_str(), global_timing_info.result_fd);
        }
        fclose(global_timing_info.result_fd);
        delete global_timing_info.timer;
    }

}