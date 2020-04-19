//
// Created by quepas on 17/04/2020.
//

#include <iostream>
#include "PEPWriter.hpp"
#include "PEPFormat.hpp"

#if defined __cpp_lib_filesystem
#include <filesystem>
namespace fs = std::filesystem;
#else

#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;
#endif

using std::cout;
using std::cerr;
using std::endl;
using std::ios;
using std::string;
using std::vector;

namespace peptalk::io {

    bool PEPWriter::Open(const string &file_name) {
        if (fs::exists(file_name)) {
            file_stream.open(file_name, ios::binary | ios::in | ios::out);
            file_stream.seekg(0, ios::beg);
            auto file_size = fs::file_size(fs::path(file_name));
            // Can the file has at least format tag, version, and number of traces?
            if (file_size >= (sizeof(char) * PEP_FORMAT_TAG.size()
                              + sizeof(unsigned int)
                              + sizeof(unsigned short int))) {
                string format_name;
                format_name.resize(PEP_FORMAT_TAG.size());
                file_stream.read((char *) format_name.c_str(), PEP_FORMAT_TAG.size());
                if (format_name != PEP_FORMAT_TAG) {
                    cerr << "Not a PEP file" << endl;
                    file_stream.close();
                    return false;
                }
                unsigned int pep_format_version;
                file_stream.read((char *) &pep_format_version, sizeof(unsigned int));
                pos_num_traces = file_stream.tellg();

                unsigned short int num_of_profiles;
                file_stream.read((char *) &num_of_profiles, sizeof(unsigned short int));
                num_profiles = num_of_profiles;

                file_stream.seekg(0, ios::end);
                pos_last_trace = file_stream.tellg();
                file_stream.seekp(pos_last_trace);
            }
        } else {
            file_stream.open(file_name, ios::binary | ios::out);
            if (!file_stream.is_open()) {
                return false;
            }
            WriteMagicNumbers();
        }
        return true;
    }

    void PEPWriter::Close() {
        file_stream.close();
    }

    void PEPWriter::StartProfile(const string &header, const vector<string> &perf_events) {
        file_stream.write(PEP_START.c_str(), PEP_START.size());
        WriteHeader(header);
        WritePerfEvents(perf_events);
        pos_measurements_size = file_stream.tellp();
        num_measurements = 0;
        file_stream.write((char *) &num_measurements, sizeof(unsigned int));
    }

    void PEPWriter::WriteMeasurements(long long int *values, size_t num) {
        num_measurements += num;
        file_stream.write((char *) values, sizeof(long long int) * num);
    }

    void PEPWriter::WriteAddress(long long int address) {
        num_measurements++;
        file_stream.write((char *) &address, sizeof(long long int));
    }

    void PEPWriter::FinishProfile() {
        file_stream.write(PEP_END.c_str(), PEP_END.size());
        pos_last_trace = file_stream.tellp();
        file_stream.seekp(pos_measurements_size);
        file_stream.write((char *) &num_measurements, sizeof(unsigned int));
        IncreaseNumProfiles();
        file_stream.seekp(pos_last_trace);
    }

    void PEPWriter::WriteMagicNumbers() {
        file_stream.write(PEP_FORMAT_TAG.c_str(), PEP_FORMAT_TAG.size());
        file_stream.write((char *) &PEP_VERSION, sizeof(unsigned int));
        pos_num_traces = file_stream.tellp();
        file_stream.write((char *) &num_profiles, sizeof(unsigned short int));
    }

    void PEPWriter::IncreaseNumProfiles() {
        file_stream.seekp(pos_num_traces);
        ++num_profiles;
        file_stream.write((char *) &num_profiles, sizeof(unsigned short int));
        file_stream.seekp(pos_last_trace);
    }

    void PEPWriter::WriteHeader(const string& header) {
        unsigned short int header_size = header.size();
        file_stream.write((char *) &header_size, sizeof(unsigned short int));
        file_stream.write(header.data(), header_size);
    }

    void PEPWriter::WritePerfEvents(const vector<string>& perf_events) {
        unsigned char num_perf_events = perf_events.size();
        file_stream.write((char *) &num_perf_events, sizeof(unsigned char));
        for (size_t idx = 0; idx < num_perf_events; ++idx) {
            auto perf_event = perf_events[idx];
            unsigned char perf_event_length = perf_event.size();
            file_stream.write((char *) &perf_event_length, sizeof(unsigned char));
            file_stream.write(perf_event.c_str(), perf_event.size());
        }
    }

}
