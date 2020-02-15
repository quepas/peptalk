//
// Created by quepas on 14/02/2020.
//

#ifndef PEP_TALK_PROFILING_HPP
#define PEP_TALK_PROFILING_HPP

#include <string>
#include <vector>
#include <functional>

namespace peptalk::profiling {

    bool
    Init(const std::string &overflow_event, int overflow_threshold, const std::vector<std::string> &measured_events,
         bool include_instruction_address, const std::string &profiling_output_path,
         const std::function<void(const std::string &, const std::string &)> &OnErrorOrWarning);

    bool Start(const std::string &trace_header,
               const std::function<void(const std::string &, const std::string &)> &OnErrorOrWarning);

    bool Stop(const std::function<void(const std::string &, const std::string &)> &OnErrorOrWarning);

}

#endif //PEP_TALK_PROFILING_HPP
