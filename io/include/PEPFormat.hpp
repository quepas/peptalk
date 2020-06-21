#ifndef PEP_TALK_PEPFORMAT_HPP
#define PEP_TALK_PEPFORMAT_HPP

#include <exception>
#include <string>
#include <optional>
#include <utility>
#include <vector>

namespace peptalk::io {

    static const unsigned int PEP_VERSION = 2;
    static const std::string PEP_FORMAT_TAG = "pep";
    static const std::string PEP_START = "@start";
    static const std::string PEP_END = "@end";

    typedef unsigned long long int inst_address_t;
    typedef long long int perf_event_t;

    struct ProfileHeader {
        std::string label;
        std::string sampling_event;
        std::vector<std::string> events;
    };

    struct Observation {
        perf_event_t sampling_time;
        std::vector<perf_event_t> values;
        std::optional<inst_address_t> address;
    };

    class InvalidFormat : public std::exception {
    public:
        explicit InvalidFormat(std::string msg) : msg(std::move(msg)) {}

        const char *what() const noexcept override {
            return msg.c_str();
        }

    private:
        const std::string msg;
    };

}

#endif //PEP_TALK_PEPFORMAT_HPP
