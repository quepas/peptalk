#include "PEPFormat.hpp"
#include "PEPReader.hpp"

#include <iostream>

using std::cerr;
using std::endl;
using std::function;
using std::ios;
using std::ios;
using std::string;
using std::vector;

namespace peptalk::io {

    bool PEPReader::Open(const string &file_name) {
        in_file_stream.open(file_name, ios::binary | ios::in);
        try {
            ReadFileHeader();
        } catch (const InvalidFormat& ex) {
            in_file_stream.close();
            return false;
        }
        return in_file_stream.is_open();
    }

    bool PEPReader::ReadProfiles(const function<bool(
            const ProfileHeader& header,
            const vector<Observation>& observations)> &on_new_profile) {
        try {
            for (size_t idx = 0; idx < num_profiles; ++idx) {
                ProfileHeader header;
                ReadProfileStart();
                ReadProfileHeader(header);
                auto num_observations = ReadNumObservations();
                vector<Observation> observations;
                observations.reserve(num_observations);

                for (auto observation_idx = 0; observation_idx < num_observations; ++observation_idx) {
                    Observation new_observation;
                    ReadObservation(new_observation, header.events.size());
                    observations.push_back(new_observation);
                }
                ReadProfileEnd();
                if (!on_new_profile(header, observations)) {
                    return false;
                }
            }
        } catch (InvalidFormat &ex) {
            cerr << "Ill-formatted file: " << ex.what() << endl;
            return false;
        }
        return true;
    }

    uint16_t PEPReader::GetNumProfiles() const {
        return num_profiles;
    }

    uint16_t PEPReader::GetPepFormatVersion() const {
        return pep_format_version;
    }

    void PEPReader::ReadFileHeader() {
        string format_name;
        format_name.resize(PEP_FORMAT_TAG.size());
        in_file_stream.read((char *) format_name.c_str(), PEP_FORMAT_TAG.size());
        if (format_name != PEP_FORMAT_TAG) {
            throw InvalidFormat("Not a PEP format at the beginning.");
        }
        in_file_stream.read((char *) &pep_format_version, sizeof(uint16_t));
        in_file_stream.read((char *) &num_profiles, sizeof(uint16_t));
        in_file_stream.read((char *) &has_instruction_address, sizeof(bool));
    }

    void PEPReader::ReadProfileStart() {
        string profile_start;
        profile_start.resize(PEP_START.size());
        in_file_stream.read((char *) profile_start.c_str(), PEP_START.size());
        if (profile_start != PEP_START) {
            throw InvalidFormat(string("Not a profile start (") + PEP_START + ")");
        }
    }

    void PEPReader::ReadProfileEnd() {
        string profile_end;
        profile_end.resize(PEP_END.size());
        in_file_stream.read((char *) profile_end.c_str(), PEP_END.size());
        if (profile_end != PEP_END) {
            throw InvalidFormat(string("Not a profile end (") + PEP_END + ")");
        }
    }

    void PEPReader::ReadProfileHeader(ProfileHeader& profile_header) {
        uint16_t label_size;
        in_file_stream.read((char *) &label_size, sizeof(uint16_t));
        profile_header.label.resize(label_size);
        in_file_stream.read((char *) profile_header.label.c_str(), label_size);

        uint8_t num_perf_events;
        in_file_stream.read((char *) &num_perf_events, sizeof(uint8_t));

        vector<string> performance_events;
        performance_events.reserve(num_perf_events);
        for (size_t idx = 0; idx < num_perf_events; ++idx) {
            uint8_t perf_event_size;
            in_file_stream.read((char *) &perf_event_size, sizeof(uint8_t));
            string perf_event;
            perf_event.resize(perf_event_size);
            in_file_stream.read((char *) perf_event.c_str(), perf_event_size);
            performance_events.push_back(perf_event);
        }
        profile_header.sampling_event = performance_events.front();
        performance_events.erase(performance_events.begin());
        profile_header.events = performance_events;
    }

    uint32_t PEPReader::ReadNumObservations() {
        uint32_t num_observations;
        in_file_stream.read((char *) &num_observations, sizeof(uint32_t));
        return num_observations;
    }

    void PEPReader::ReadObservation(Observation &observation, size_t num_performance_events) {
        // Read sampling time
        in_file_stream.read((char *) &observation.sampling_time, sizeof(perf_event_t));
        // Read the rest of performance events
        observation.values.resize(num_performance_events);
        in_file_stream.read((char *) observation.values.data(), num_performance_events * sizeof(perf_event_t));
        if (has_instruction_address) {
            inst_address_t value;
            in_file_stream.read((char *) &value, sizeof(inst_address_t));
            observation.address = value;
        }
    }

    void PEPReader::Close() {
        in_file_stream.close();
    }

    bool PEPReader::GetHasInstructionAddressFlag() const {
        return has_instruction_address;
    }

}