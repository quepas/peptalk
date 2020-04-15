//
// Created by quepas on 15/04/2020.
//

#ifndef PEP_TALK_COUNTING_HPP
#define PEP_TALK_COUNTING_HPP

#include <string>
#include <vector>
#include <functional>

typedef const std::function<void(const std::string &, const std::string &)> error_callback_type;

namespace peptalk::counting {

    bool Init(const std::string &result_file,
              const std::vector<std::string> &parameters_names,
              const std::vector<std::string> &performance_events,
              unsigned int num_measurements,
              error_callback_type &OnErrorOrWarning);

    bool Start(const std::vector<std::string> &parameters, error_callback_type &OnErrorOrWarning);

    bool Stop(error_callback_type &OnErrorOrWarning);

    bool Close(error_callback_type &OnErrorOrWarning);

}

#endif //PEP_TALK_COUNTING_HPP
