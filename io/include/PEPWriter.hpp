//
// Created by quepas on 17/04/2020.
//

#ifndef PEP_TALK_PEPWRITER_HPP
#define PEP_TALK_PEPWRITER_HPP

#include <fstream>
#include <string>
#include <vector>

namespace peptalk::io {

    class PEPWriter {
    public:

        bool Open(const std::string &file_name);

        void Close();

        void StartProfile(const std::string &header, const std::vector<std::string> &perf_events);

        void WriteMeasurements(long long int *values, size_t num);

        void WriteMeasurements(std::vector<long long int> values);

        void WriteAddress(long long int address);

        void FinishProfile();

    private:
        std::fstream file_stream;
        unsigned short int num_profiles = 0;
        unsigned int num_measurements = 0;

        std::ios::pos_type pos_num_traces, pos_measurements_size, pos_last_trace;

        void WriteMagicNumbers();

        void IncreaseNumProfiles();

        void WriteHeader(const std::string &header);

        void WritePerfEvents(const std::vector<std::string> &perf_events);

    };

}

#endif //PEP_TALK_PEPWRITER_HPP
