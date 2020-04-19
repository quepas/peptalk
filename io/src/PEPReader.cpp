//
// Created by quepas on 17/04/2020.
//

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
        return in_file_stream.is_open();
    }

    bool PEPReader::ReadProfiles(function<bool(
            const string& header,
            const vector<string>& performance_events,
            const vector<long long int>& measurements)> onNewProfile) {
        try {
            CheckMagicNumbers();
            ReadNumProfiles();

            for (size_t idx = 0; idx < num_profiles; ++idx) {
                CheckProfileStart();
                auto header = ReadHeader();
                vector<string> performance_events;
                ReadPerformanceEvents(performance_events);
                auto num_measurements = ReadNumMeasurements();
                vector<long long int> measurements;
                ReadMeasurements(measurements, num_measurements);
                CheckProfileEnd();
                if (!onNewProfile(header, performance_events, measurements)) {
                    return false;
                }
            }
        } catch (InvalidFormat &ex) {
            cerr << "Ill-formatted file: " << ex.what() << endl;
            return false;
        }
        return true;
    }

    unsigned short PEPReader::GetNumProfiles() const {
        return num_profiles;
    }

    unsigned int PEPReader::getPepFormatVersion() const {
        return pep_format_version;
    }

    void PEPReader::CheckMagicNumbers() {
        string format_name;
        format_name.resize(PEP_FORMAT_TAG.size());
        in_file_stream.read((char *) format_name.c_str(), PEP_FORMAT_TAG.size() );
        if (format_name != PEP_FORMAT_TAG) {
            throw InvalidFormat("Not a PEP format at the beginning.");
        }
        in_file_stream.read((char *) &pep_format_version, sizeof(unsigned int));
    }

    void PEPReader::ReadNumProfiles() {
        in_file_stream.read((char *) &num_profiles, sizeof(unsigned short int));
    }

    void PEPReader::CheckProfileStart() {
        string profile_start;
        profile_start.resize(PEP_START.size());
        in_file_stream.read((char *) profile_start.c_str(), PEP_START.size());
        if (profile_start != PEP_START) {
            throw InvalidFormat("Not a profile start (" + PEP_START + ")");
        }
    }

    void PEPReader::CheckProfileEnd() {
        string profile_end;
        profile_end.resize(PEP_END.size());
        in_file_stream.read((char *) profile_end.c_str(), PEP_END.size());
        if (profile_end != PEP_END) {
            throw InvalidFormat("Not a profile end (" + PEP_END + ")");
        }
    }

    string PEPReader::ReadHeader() {
        unsigned short int header_size;
        in_file_stream.read((char *) &header_size, sizeof(unsigned short int));
        string header;
        header.resize(header_size);
        in_file_stream.read((char *) header.c_str(), header_size);
        return header;
    }

    void PEPReader::ReadPerformanceEvents(vector<string>& performance_events) {
        unsigned char num_perf_events;
        in_file_stream.read((char *) &num_perf_events, sizeof(unsigned char));

        for (size_t idx = 0; idx < num_perf_events; ++idx) {
            unsigned char perf_event_size;
            in_file_stream.read((char *) &perf_event_size, sizeof(unsigned char));
            string perf_event;
            perf_event.resize(perf_event_size);
            in_file_stream.read((char *) perf_event.c_str(), perf_event_size);
            performance_events.push_back(perf_event);
        }
    }

    unsigned int PEPReader::ReadNumMeasurements() {
        unsigned int num_measurements;
        in_file_stream.read((char *) &num_measurements, sizeof(unsigned int));
        return num_measurements;
    }

    void PEPReader::ReadMeasurements(vector<long long int>& measurements, unsigned int num_measurements) {
        measurements.resize(num_measurements);
        in_file_stream.read((char *) measurements.data(), num_measurements * sizeof(long long int));
    }

    void PEPReader::Close() {
        in_file_stream.close();
    }

}