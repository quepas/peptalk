#ifndef PEP_TALK_PEPWRITER_HPP
#define PEP_TALK_PEPWRITER_HPP

#include "PEPFormat.hpp"

#include <fstream>
#include <string>
#include <vector>

namespace peptalk::io {

    class PEPWriter {
    public:

        bool Open(const std::string &file_name, bool write_instruction_address = false);

        void StartProfile(const std::string &profile_label, const std::vector<std::string> &perf_events);

        void WriteObservation(perf_event_t *values, size_t num_values);

        void WriteObservation(perf_event_t *values, size_t num_values, const inst_address_t& address);

        void WriteObservation(std::vector<perf_event_t> values, const inst_address_t& address);

        void FinishProfile();

        void Close();

    private:
        std::fstream file_stream;
        uint16_t num_profiles = 0;
        uint32_t num_observations = 0;
        bool has_instruction_address = false;

        std::ios::pos_type position_num_profiles, position_num_observations, position_last_profile;

        void WriteFileHeader();

        void IncreaseNumProfiles();

        void WriteProfileLabel(const std::string &profile_label);

        void WritePerfEvents(const std::vector<std::string> &perf_events);

    };

}

#endif //PEP_TALK_PEPWRITER_HPP
