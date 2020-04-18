//
// Created by quepas on 17/04/2020.
//

#include <iostream>
#include "PEPWriter.hpp"
#include "PEPFormat.hpp"

using std::cerr;
using std::endl;
using std::ios;
using std::string;
using std::vector;

namespace peptalk::io {

    bool PEPWriter::Open(const string& file_name) {
        out_file.open(file_name, ios::binary | ios::out);
        if (!out_file.is_open()) {
            return false;
        }
        WriteMagicNumbers();
        return true;
    }

    void PEPWriter::Close() {
        out_file.close();
    }

    void PEPWriter::StartProfile(const string& header, const vector<string>& perf_events) {
        out_file.write(PEP_START.c_str(), PEP_START.size());
        WriteHeader(header);
        WritePerfEvents(perf_events);
        pos_measurements_size = out_file.tellp();
        num_measurements = 0;
        out_file.write((char*) &num_measurements, sizeof(unsigned int));
    }

    void PEPWriter::WriteMeasurements(long long int *values, size_t num) {
        num_measurements += num;
        out_file.write((char*) values, sizeof(long long int) * num);
    }

    void PEPWriter::WriteAddress(long long int address) {
        num_measurements++;
        out_file.write((char*)&address, sizeof(long long int));
    }

    void PEPWriter::FinishProfile() {
        out_file.write(PEP_END.c_str(), PEP_END.size());
        pos_last_trace = out_file.tellp();
        out_file.seekp(pos_measurements_size);
        out_file.write((char*) &num_measurements, sizeof(unsigned int));
        IncreaseNumProfiles();
        out_file.seekp(pos_last_trace);
    }

    void PEPWriter::WriteMagicNumbers() {
        out_file.write(PEP_FORMAT_TAG.c_str(), PEP_FORMAT_TAG.size());
        out_file.write((char*)&PEP_VERSION, sizeof(unsigned int));
        pos_num_traces = out_file.tellp();
        out_file.write((char*)&num_profiles, sizeof(unsigned short int));
    }

    void PEPWriter::IncreaseNumProfiles() {
        out_file.seekp(pos_num_traces);
        ++num_profiles;
        out_file.write((char*)&num_profiles, sizeof(unsigned short int));
        out_file.seekp(pos_last_trace);
    }

    void PEPWriter::WriteHeader(const string& header) {
        unsigned short int header_size = header.size();
        out_file.write((char*)&header_size, sizeof(unsigned short int));
        out_file.write(header.data(), header_size);
    }

    void PEPWriter::WritePerfEvents(const vector<string>& perf_events) {
        unsigned char num_perf_events = perf_events.size();
        out_file.write((char*)&num_perf_events, sizeof(unsigned char));
        for (size_t idx = 0; idx < num_perf_events; ++idx) {
            auto perf_event = perf_events[idx];
            unsigned char perf_event_length = perf_event.size();
            out_file.write((char*)&perf_event_length, sizeof(unsigned char));
            out_file.write(perf_event.c_str(), perf_event.size());
        }
    }

}
