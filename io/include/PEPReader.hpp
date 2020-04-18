//
// Created by quepas on 17/04/2020.
//

#ifndef PEP_TALK_PEPREADER_HPP
#define PEP_TALK_PEPREADER_HPP

#include <exception>
#include <fstream>
#include <functional>
#include <string>
#include <vector>

namespace peptalk::io {

class PEPReader {
public:

    class InvalidFormat : public std::exception {
    public:
        explicit InvalidFormat(const std::string &msg) : msg(msg) {}

        const char *what() const noexcept override {
            return msg.c_str();
        }

    private:
        const std::string &msg;
    };

    bool Open(const std::string &file_name);

    bool ReadProfiles(std::function<void(
            const std::string& header,
            const std::vector<std::string>& performance_events,
            const std::vector<long long int>& measurements)> onNewProfile);

    unsigned short GetNumProfiles() const;

    unsigned int getPepFormatVersion() const;

private:
    std::ifstream in_file;
    unsigned short int num_profiles = 0;
    unsigned int pep_format_version = 0;

    void CheckMagicNumbers();

    void ReadNumProfiles();

    void CheckProfileStart();

    void CheckProfileEnd();

    std::string ReadHeader();

    void ReadPerformanceEvents(std::vector<std::string>& performance_events);

    unsigned int ReadNumMeasurements();

    void ReadMeasurements(std::vector<long long int>& measurements, unsigned int num_measurements);

};

}

#endif //PEP_TALK_PEPREADER_HPP
