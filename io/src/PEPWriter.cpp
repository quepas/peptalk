#include "PEPWriter.hpp"

#include <iostream>
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

    bool PEPWriter::Open(const std::string &file_name, bool write_instruction_address /*=false */) {
        if (fs::exists(file_name)) {
            file_stream.open(file_name, ios::binary | ios::in | ios::out);
            file_stream.seekg(0, ios::beg);
            auto file_size = fs::file_size(fs::path(file_name));
            /* Can the file has at least:
             * 1) format tag
             * 2) version
             * 3) number of traces
             * 4) instruction address flag
             */
            if (file_size >= (sizeof(char) * PEP_FORMAT_TAG.size()
                              + sizeof(uint16_t)
                              + sizeof(uint16_t)
                              + sizeof(bool))) {
                string format_name;
                format_name.resize(PEP_FORMAT_TAG.size());
                file_stream.read((char *) format_name.c_str(), PEP_FORMAT_TAG.size());
                if (format_name != PEP_FORMAT_TAG) {
                    cerr << "Not a PEP file" << endl;
                    file_stream.close();
                    return false;
                }
                uint16_t pep_format_version;
                file_stream.read((char *) &pep_format_version, sizeof(uint16_t));

                position_num_profiles = file_stream.tellg();
                uint16_t num_of_profiles;
                file_stream.read((char *) &num_of_profiles, sizeof(uint16_t));
                num_profiles = num_of_profiles;

                file_stream.read((char *) &has_instruction_address, sizeof(bool));
                if (has_instruction_address != write_instruction_address) {
                    if (write_instruction_address) {
                        throw InvalidFormat("Existing PEP file was written without instruction address.");
                    } else {
                        throw InvalidFormat("Existing PEP file requires instruction address.");
                    }
                }

                file_stream.seekg(0, ios::end);
                position_last_profile = file_stream.tellg();
                file_stream.seekp(position_last_profile);
            }
        } else {
            has_instruction_address = write_instruction_address;
            file_stream.open(file_name, ios::binary | ios::out);
            if (!file_stream.is_open()) {
                return false;
            }
            WriteFileHeader();
        }
        return true;
    }

    void PEPWriter::Close() {
        file_stream.close();
    }

    void PEPWriter::StartProfile(const string &profile_label, const vector<string> &perf_events) {
        file_stream.write(PEP_START.c_str(), PEP_START.size());
        WriteProfileLabel(profile_label);
        WritePerfEvents(perf_events);
        position_num_observations = file_stream.tellp();
        num_observations = 0;
        file_stream.write((char *) &num_observations, sizeof(uint32_t));
    }

    void PEPWriter::WriteObservation(perf_event_t *values, size_t num_values) {
        num_observations += 1;
        file_stream.write((char *) values, sizeof(perf_event_t) * num_values);
    }

    void PEPWriter::WriteObservation(perf_event_t *values, size_t num_values, const inst_address_t &address) {
        WriteObservation(values, num_values);
        file_stream.write((char *) &address, sizeof(inst_address_t));
    }

    void PEPWriter::FinishProfile() {
        file_stream.write(PEP_END.c_str(), PEP_END.size());
        position_last_profile = file_stream.tellp();
        file_stream.seekp(position_num_observations);
        file_stream.write((char *) &num_observations, sizeof(uint32_t));
        IncreaseNumProfiles();
        file_stream.seekp(position_last_profile);
    }

    void PEPWriter::WriteFileHeader() {
        file_stream.write(PEP_FORMAT_TAG.c_str(), PEP_FORMAT_TAG.size());
        file_stream.write((char *) &PEP_VERSION, sizeof(uint16_t));
        position_num_profiles = file_stream.tellp();
        file_stream.write((char *) &num_profiles, sizeof(uint16_t));
        file_stream.write((char *) &has_instruction_address, sizeof(bool));
    }

    void PEPWriter::IncreaseNumProfiles() {
        file_stream.seekp(position_num_profiles);
        ++num_profiles;
        file_stream.write((char *) &num_profiles, sizeof(uint16_t));
        file_stream.seekp(position_last_profile);
    }

    void PEPWriter::WriteProfileLabel(const string &profile_label) {
        uint16_t label_size = profile_label.size();
        file_stream.write((char *) &label_size, sizeof(uint16_t));
        file_stream.write(profile_label.data(), label_size);
    }

    void PEPWriter::WritePerfEvents(const vector<string>& perf_events) {
        uint8_t num_perf_events = perf_events.size();
        file_stream.write((char *) &num_perf_events, sizeof(uint8_t));
        for (size_t idx = 0; idx < num_perf_events; ++idx) {
            auto perf_event = perf_events[idx];
            uint8_t perf_event_length = perf_event.size();
            file_stream.write((char *) &perf_event_length, sizeof(uint8_t));
            file_stream.write(perf_event.c_str(), perf_event.size());
        }
    }

    void PEPWriter::WriteObservation(std::vector<perf_event_t> values, const inst_address_t &address) {
        WriteObservation(values.data(), values.size(), address);
    }

}
